/* skills.c  1.2  1/8/95 */
/* Code specifically for the new skill system */

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <strings.h> /* for bzero() */
#include <stdlib.h>
#include "merc.h"
#include "magic.h"

/* command procedures needed */
DECLARE_DO_FUN(do_groups	);
DECLARE_DO_FUN(do_help		);
DECLARE_DO_FUN(do_say		);


/* used to get new skills */
void do_gain(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *trainer;
    bool foundgm, isgroup; 
    struct guildmaster_type gmdata;
    int gn = 0, sn = 0;
    int i;

    if (IS_NPC(ch))
	return;

    /* find a trainer */
    for ( trainer = ch->in_room->people; 
	  trainer != NULL; 
	  trainer = trainer->next_in_room)
	if (IS_NPC(trainer) && IS_SET(trainer->act,ACT_PRACTICE))
	    break;

    foundgm = FALSE;
    if (trainer != NULL) 
        for (i=0; guildmaster_table[i].vnum != 0; i++)
        {
            gmdata = guildmaster_table[i];
            if (gmdata.vnum == trainer->pIndexData->vnum)
            {
		foundgm = TRUE;
		break;
	    }
	}


    if (!foundgm)
    {
	send_to_char("You can't do that here.\n\r",ch);
	return;
    }

    if(gmdata.class == CLASS_OTHER && ch->class == gmdata.guild)
    {
      act("$n tells you, 'Your knowledge is beyond mine. I can't teach you.'",
	      trainer, NULL, ch, TO_VICT);
      return;
    }

    if ( (gmdata.class != CLASS_ANY && gmdata.class != CLASS_OTHER)
      && (ch->class != gmdata.class) )
    {
	send_to_char("You are the wrong class to train with this guildmaster.\n\r",ch);
	return;
    }

    if ((gmdata.guild != GUILD_ANY)
    && (ch->pcdata->guild != gmdata.guild))
    {
	act("$n tells you 'I only train members of my guild.'",
		      trainer, NULL, ch, TO_VICT);
	return;
    }


    one_argument(argument,arg);

    if ((arg[0] == '\0') || (!str_prefix(arg,"list")))
    {
	int col;
	
	col = 0;

	sprintf(buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
	             "group","cost","group","cost","group","cost");
	send_to_char(buf,ch);

	for (i = 0; i < MAX_GAIN; i++)
	{
	    if (gmdata.can_gain[i] == NULL)
	        break;

	    gn = group_lookup(gmdata.can_gain[i]);

	    isgroup = TRUE;
	    if ((gn < 0) || (group_table[gn].name == NULL))
	    {
	        gn = skill_lookup(gmdata.can_teach[i]);
		isgroup = FALSE;

                if (skill_table[gn].name == NULL)
                {
                    sprintf( buf, 
                           "[*****] BUG: Group/Skill lookup failed for %s.",
                           gmdata.can_gain[i] );
                    log_string( buf );
                    continue;
		}
            }

	    if (isgroup && !ch->pcdata->group_known[gn]
	    &&  group_table[gn].rating[ch->class] > 0)
	    {
		sprintf(buf,"%-18s %-5d ",
		    group_table[gn].name,group_table[gn].rating[ch->class]);
		send_to_char(buf,ch);
		if (++col % 3 == 0)
		    send_to_char("\n\r",ch);
	    }
	}
	if (col % 3 != 0)
	    send_to_char("\n\r",ch);
	
	send_to_char("\n\r",ch);		

	col = 0;

        sprintf(buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
                     "skill","cost","skill","cost","skill","cost");
        send_to_char(buf,ch);
 
        for (i = 0; i < MAX_GAIN; i++)
        {
	    if (gmdata.can_gain[i] == NULL)
	        break;

	    sn = skill_lookup(gmdata.can_gain[i]);

            if ((sn < 0) || (skill_table[sn].name == NULL))
	        continue;

            if (!ch->pcdata->learned[sn]
            &&  skill_table[sn].rating[ch->class] > 0
	    &&  skill_table[sn].spell_fun == spell_null)
            {
                sprintf(buf,"%-18s %-5d ",
                    skill_table[sn].name,skill_table[sn].rating[ch->class]);
                send_to_char(buf,ch);
                if (++col % 3 == 0)
                    send_to_char("\n\r",ch);
            }
        }
        if (col % 3 != 0)
            send_to_char("\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"convert"))
    {
	if (ch->practice < 10)
	{
	    act("$N tells you 'You are not yet ready.'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}

	act("$N helps you apply your practice to training",
		ch,NULL,trainer,TO_CHAR);
	ch->practice -= 10;
	ch->train +=1 ;
	return;
    }

    if (!str_prefix(arg,"points"))
    {
	if (ch->train < 2)
	{
	    act("$N tells you 'You are not yet ready.'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}

	if (ch->pcdata->points <= 40)
	{
	    act("$N tells you 'There would be no point in that.'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}

	act("$N trains you, and you feel more at ease with your skills.",
	    ch,NULL,trainer,TO_CHAR);

	ch->train -= 2;
	ch->pcdata->points -= 1;
	ch->exp = exp_per_level(ch,ch->pcdata->points) * ch->level;
	return;
    }

    /* else add a group/skill */
    for ( i = 0, gn = -1 ; (i < MAX_GAIN) && (gn < 0); i++ )
    {
        if ((gmdata.can_gain[i] != NULL) 
        && !str_prefix(argument,gmdata.can_gain[i]))
            gn = group_lookup(gmdata.can_gain[i]);
    }

    if (gn > 0)
    {
	if (ch->pcdata->group_known[gn])
	{
	    act("$N tells you 'You already know that group!'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}

	if (group_table[gn].rating[ch->class] <= 0)
	{
	    act("$N tells you 'That group is beyond your powers.'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}

	if (ch->train < group_table[gn].rating[ch->class])
	{
	    act("$N tells you 'You are not yet ready for that group.'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}

	/* add the group */
	gn_add(ch,gn);
	act("$N trains you in the art of $t",
	    ch,group_table[gn].name,trainer,TO_CHAR);
	ch->train -= group_table[gn].rating[ch->class];
	return;
    }

    for ( i = 0, sn = -1; (i < MAX_GAIN) && (sn < 0); i++ )
    {
        if ((gmdata.can_gain[i] != NULL) 
        && !str_prefix(argument,gmdata.can_gain[i]))
            sn = skill_lookup(gmdata.can_gain[i]);
    }

    if (sn > -1)
    {
	if (skill_table[sn].spell_fun != spell_null)
	{
	    act("$N tells you 'You must learn the full group.'",
		ch,NULL,trainer,TO_CHAR);
	    return;
	}
	    

        if (ch->pcdata->learned[sn])
        {
            act("$N tells you 'You already know that skill!'",
                ch,NULL,trainer,TO_CHAR);
            return;
        }
 
        if (skill_table[sn].rating[ch->class] <= 0)
        {
            act("$N tells you 'That skill is beyond your powers.'",
                ch,NULL,trainer,TO_CHAR);
            return;
        }
 
        if (ch->train < skill_table[sn].rating[ch->class])
        {
            act("$N tells you 'You are not yet ready for that skill.'",
                ch,NULL,trainer,TO_CHAR);
            return;
        }
 
        /* add the skill */
	ch->pcdata->learned[sn] = 1;
        act("$N trains you in the art of $t.",
            ch,skill_table[sn].name,trainer,TO_CHAR);
        ch->train -= skill_table[sn].rating[ch->class];
        return;
    }

    act("$N tells you 'I do not know that skill.'",ch,NULL,trainer,TO_CHAR);
}
    



/* RT spells and skills show the players spells (or skills) */

void do_spells(CHAR_DATA *ch, char *argument)
{
    char spell_list[LEVEL_HERO1][MAX_STRING_LENGTH];
    char spell_columns[LEVEL_HERO1];
    int sn,lev,mana;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
      return;

    /* initilize data */
    for (lev = 0; lev < LEVEL_HERO1; lev++)
    {
	spell_columns[lev] = 0;
	spell_list[lev][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++)
    {
      if (skill_table[sn].name == NULL)
	break;

      if (skill_table[sn].skill_level[ch->class] < LEVEL_HERO1 &&
	  skill_table[sn].spell_fun != spell_null &&
	  ch->pcdata->learned[sn] > 0)
      {
	found = TRUE;
	lev = skill_table[sn].skill_level[ch->class];
	if (ch->level < lev)
	  sprintf(buf,"%-18s  n/a      ", skill_table[sn].name);
	else
	{
	  mana = UMAX(skill_table[sn].min_mana,
		      100/(2 + ch->level - lev));
	  sprintf(buf,"%-18s  %3d mana  ",skill_table[sn].name,mana);
	}

	if (spell_list[lev][0] == '\0')
	  sprintf(spell_list[lev],"\n\rLevel %2d: %s",lev,buf);
	else /* append */
	{
	  if ( ++spell_columns[lev] % 2 == 0)
	    strcat(spell_list[lev],"\n\r          ");
	  strcat(spell_list[lev],buf);
	}
      }
    }

    /* return results */

    if (!found)
    {
      send_to_char("You know no spells.\n\r",ch);
      return;
    }

    for (lev = 0; lev < LEVEL_HERO1; lev++)
      if (spell_list[lev][0] != '\0') {
	 if(ch->lines)
	   page_to_char(spell_list[lev],ch);
	 else
	   send_to_char(spell_list[lev],ch);
    }

    send_to_char("\n\r",ch);
}

void do_skills(CHAR_DATA *ch, char *argument)
{
    char skill_list[LEVEL_HERO1][MAX_STRING_LENGTH];
    char skill_columns[LEVEL_HERO1];
    int sn,lev;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
      return;

    /* initilize data */
    for (lev = 0; lev < LEVEL_HERO1; lev++)
    {
	skill_columns[lev] = 0;
	skill_list[lev][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++)
    {
      if (skill_table[sn].name == NULL )
	break;


      if (skill_table[sn].skill_level[ch->class] < LEVEL_HERO1 &&
	  skill_table[sn].spell_fun == spell_null &&
	  ch->pcdata->learned[sn] > 0)
      {
	found = TRUE;
	lev = skill_table[sn].skill_level[ch->class];
	if (ch->level < lev)
	  sprintf(buf,"%-18s n/a      ", skill_table[sn].name);
	else
	  sprintf(buf,"%-18s %3d%%      ",skill_table[sn].name,
					 ch->pcdata->learned[sn]);

	if (skill_list[lev][0] == '\0')
	  sprintf(skill_list[lev],"\n\rLevel %2d: %s",lev,buf);
	else /* append */
	{
	  if ( ++skill_columns[lev] % 2 == 0)
	    strcat(skill_list[lev],"\n\r          ");
	  strcat(skill_list[lev],buf);
	}
      }
    }

    /* return results */

    if (!found)
    {
      send_to_char("You know no skills.\n\r",ch);
      return;
    }

    for (lev = 0; lev < LEVEL_HERO1; lev++)
      if (skill_list[lev][0] != '\0')
	send_to_char(skill_list[lev],ch);
    send_to_char("\n\r",ch);
}


/* shows skills, groups and costs (only if not bought) */
void list_group_costs(CHAR_DATA *ch)
{
    char buf[100];
    int gn,sn,col;

    if (IS_NPC(ch))
	return;

    col = 0;

    sprintf(buf,"%-18s %-5s %-18s %-5s %-18s %-5s\n\r","group","cp","group","cp","group","cp");
    send_to_char(buf,ch);

    for (gn = 0; gn < MAX_GROUP; gn++)
    {
	if (group_table[gn].name == NULL)
	    break;

        if (!ch->gen_data->group_chosen[gn] 
	&&  !ch->pcdata->group_known[gn]
	&&  group_table[gn].rating[ch->class] > 0)
	{
	    sprintf(buf,"%-18s %-5d ",group_table[gn].name,
				    group_table[gn].rating[ch->class]);
	    send_to_char(buf,ch);
	    if (++col % 3 == 0)
		send_to_char("\n\r",ch);
	}
    }
    if ( col % 3 != 0 )
        send_to_char( "\n\r", ch );
    send_to_char("\n\r",ch);

    col = 0;
 
    sprintf(buf,"%-18s %-5s %-18s %-5s %-18s %-5s\n\r","skill","cp","skill","cp","skill","cp");
    send_to_char(buf,ch);
 
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL)
            break;
 
        if (!ch->gen_data->skill_chosen[sn] 
	&&  ch->pcdata->learned[sn] == 0
	&&  skill_table[sn].spell_fun == spell_null
	&&  skill_table[sn].rating[ch->class] > 0)
        {
            sprintf(buf,"%-18s %-5d ",skill_table[sn].name,
                                    skill_table[sn].rating[ch->class]);
            send_to_char(buf,ch);
            if (++col % 3 == 0)
                send_to_char("\n\r",ch);
        }
    }
    if ( col % 3 != 0 )
        send_to_char( "\n\r", ch );
    send_to_char("\n\r",ch);

    sprintf(buf,"Creation points: %d\n\r",ch->pcdata->points);
    send_to_char(buf,ch);
    sprintf(buf,"Experience per level: %d\n\r",
	    exp_per_level(ch,ch->gen_data->points_chosen));
    send_to_char(buf,ch);
    return;
}


void list_group_chosen(CHAR_DATA *ch)
{
    char buf[100];
    int gn,sn,col;
 
    if (IS_NPC(ch))
        return;
 
    col = 0;
 
    sprintf(buf,"%-18s %-5s %-18s %-5s %-18s %-5s","group","cp","group","cp","group","cp\n\r");
    send_to_char(buf,ch);
 
    for (gn = 0; gn < MAX_GROUP; gn++)
    {
        if (group_table[gn].name == NULL)
            break;
 
        if (ch->gen_data->group_chosen[gn] 
	&&  group_table[gn].rating[ch->class] > 0)
        {
            sprintf(buf,"%-18s %-5d ",group_table[gn].name,
                                    group_table[gn].rating[ch->class]);
            send_to_char(buf,ch);
            if (++col % 3 == 0)
                send_to_char("\n\r",ch);
        }
    }
    if ( col % 3 != 0 )
        send_to_char( "\n\r", ch );
    send_to_char("\n\r",ch);
 
    col = 0;
 
    sprintf(buf,"%-18s %-5s %-18s %-5s %-18s %-5s","skill","cp","skill","cp","skill","cp\n\r");
    send_to_char(buf,ch);
 
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL)
            break;
 
        if (ch->gen_data->skill_chosen[sn] 
	&&  skill_table[sn].rating[ch->class] > 0)
        {
            sprintf(buf,"%-18s %-5d ",skill_table[sn].name,
                                    skill_table[sn].rating[ch->class]);
            send_to_char(buf,ch);
            if (++col % 3 == 0)
                send_to_char("\n\r",ch);
        }
    }
    if ( col % 3 != 0 )
        send_to_char( "\n\r", ch );
    send_to_char("\n\r",ch);
 
    sprintf(buf,"Creation points: %d\n\r",ch->gen_data->points_chosen);
    send_to_char(buf,ch);
    sprintf(buf,"Experience per level: %d\n\r",
	    exp_per_level(ch,ch->gen_data->points_chosen));
    send_to_char(buf,ch);
    return;
}
/*
int exp_per_level(CHAR_DATA *ch, int points)
{
    int expl,inc;

    if (IS_NPC(ch))
	return 1000;

    expl = 2000;
    inc = 500;

    if (points < 40)
	return 2000 * pc_race_table[ch->race].class_mult[ch->class]/100;


    points -= 40;

    while (points > 9)
    {
	expl += inc;
	points -= 10;
	if (points > 9)
	{
	    expl += inc;
	    inc *= 2;
	    points -= 10;
	}
    }

    expl += points * inc / 10;

    return expl * pc_race_table[ch->race].class_mult[ch->class]/100;
}
*/
int exp_per_level(CHAR_DATA *ch, int points)
{
    int expl,inc;

    if (IS_NPC(ch))
	return 1000;

    expl = 2000;

    if(ch->pcdata->guild != GUILD_NONE)
      inc = pc_race_table[ch->race].class_mult2[ch->pcdata->guild];
    else
      inc = 0;

    return 2000 + pc_race_table[ch->race].class_mult[ch->class]
		+ inc ;

}

/* this procedure handles the input parsing for the skill generator */
bool parse_gen_groups(CHAR_DATA *ch,char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int gn,sn,i;

    if (argument[0] == '\0')
	return FALSE;

    argument = one_argument(argument,arg);

    if (!str_prefix(arg,"help"))
    {
	if (argument[0] == '\0')
	{
	    do_help(ch,"group help");
	    return TRUE;
	}

        do_help(ch,argument);
	return TRUE;
    }

    if (!str_prefix(arg,"add"))
    {
	if (argument[0] == '\0')
	{
	    send_to_char("You must provide a skill name.\n\r",ch);
	    return TRUE;
	}

	gn = group_lookup(argument);
	if (gn != -1)
	{
	    if (ch->gen_data->group_chosen[gn]
	    ||  ch->pcdata->group_known[gn])
	    {
		send_to_char("You already know that group!\n\r",ch);
		return TRUE;
	    }

	    if (group_table[gn].rating[ch->class] < 1)
	    {
	  	send_to_char("That group is not available.\n\r",ch);
	 	return TRUE;
	    }

	    sprintf(buf,"%s group added\n\r",group_table[gn].name);
	    send_to_char(buf,ch);
	    ch->gen_data->group_chosen[gn] = TRUE;
	    ch->gen_data->points_chosen += group_table[gn].rating[ch->class];
	    gn_add(ch,gn);
	    ch->pcdata->points += group_table[gn].rating[ch->class];
	    return TRUE;
	}

	sn = skill_lookup(argument);
	if (sn != -1)
	{
	    if (ch->gen_data->skill_chosen[sn]
	    ||  ch->pcdata->learned[sn] > 0)
	    {
		send_to_char("You already know that skill!\n\r",ch);
		return TRUE;
	    }

	    if (skill_table[sn].rating[ch->class] < 1
	    ||  skill_table[sn].spell_fun != spell_null)
	    {
		send_to_char("That skill is not available.\n\r",ch);
		return TRUE;
	    }
	    sprintf(buf, "%s skill added\n\r",skill_table[sn].name);
	    send_to_char(buf,ch);
	    ch->gen_data->skill_chosen[sn] = TRUE;
	    ch->gen_data->points_chosen += skill_table[sn].rating[ch->class];
	    ch->pcdata->learned[sn] = 1;
	    ch->pcdata->points += skill_table[sn].rating[ch->class];
	    return TRUE;
	}

	send_to_char("No skills or groups by that name...\n\r",ch);
	return TRUE;
    }

    if (!strcmp(arg,"drop"))
    {
	if (argument[0] == '\0')
  	{
	    send_to_char("You must provide a skill to drop.\n\r",ch);
	    return TRUE;
	}

	gn = group_lookup(argument);
	if (gn != -1 && ch->gen_data->group_chosen[gn])
	{
	    send_to_char("Group dropped.\n\r",ch);
	    ch->gen_data->group_chosen[gn] = FALSE;
	    ch->gen_data->points_chosen -= group_table[gn].rating[ch->class];
	    gn_remove(ch,gn);
	    for (i = 0; i < MAX_GROUP; i++)
	    {
		if (ch->gen_data->group_chosen[gn])
		    gn_add(ch,gn);
	    }
	    ch->pcdata->points -= group_table[gn].rating[ch->class];
	    return TRUE;
	}

	sn = skill_lookup(argument);
	if (sn != -1 && ch->gen_data->skill_chosen[sn])
	{
	    send_to_char("Skill dropped.\n\r",ch);
	    ch->gen_data->skill_chosen[sn] = FALSE;
	    ch->gen_data->points_chosen -= skill_table[sn].rating[ch->class];
	    ch->pcdata->learned[sn] = 0;
	    ch->pcdata->points -= skill_table[sn].rating[ch->class];
	    return TRUE;
	}

	send_to_char("You haven't bought any such skill or group.\n\r",ch);
	return TRUE;
    }

    if (!str_prefix(arg,"premise"))
    {
	do_help(ch,"premise");
	return TRUE;
    }

    if (!str_prefix(arg,"list"))
    {
	list_group_costs(ch);
	return TRUE;
    }

    if (!str_prefix(arg,"learned"))
    {
	list_group_chosen(ch);
	return TRUE;
    }

    if (!str_prefix(arg,"info"))
    {
	do_groups(ch,argument);
	return TRUE;
    }

    return FALSE;
}
	    
	


        

/* shows all groups, or the sub-members of a group */
void do_groups(CHAR_DATA *ch, char *argument)
{
    char buf[100];
    int gn,sn,col;

    if (IS_NPC(ch))
	return;

    col = 0;

    if (argument[0] == '\0')
    {   /* show all groups */
	
	for (gn = 0; gn < MAX_GROUP; gn++)
        {
	    if (group_table[gn].name == NULL)
		break;
	    if (ch->pcdata->group_known[gn])
	    {
		sprintf(buf,"%-20s ",group_table[gn].name);
		send_to_char(buf,ch);
		if (++col % 3 == 0)
		    send_to_char("\n\r",ch);
	    }
        }
        if ( col % 3 != 0 )
            send_to_char( "\n\r", ch );
        sprintf(buf,"Creation points: %d\n\r",ch->pcdata->points);
	send_to_char(buf,ch);
	return;
     }

     if (!str_cmp(argument,"all"))    /* show all groups */
     {
        for (gn = 0; gn < MAX_GROUP; gn++)
        {
            if (group_table[gn].name == NULL)
                break;
	    sprintf(buf,"%-20s ",group_table[gn].name);
            send_to_char(buf,ch);
	    if (++col % 3 == 0)
            	send_to_char("\n\r",ch);
        }
        if ( col % 3 != 0 )
            send_to_char( "\n\r", ch );
	return;
     }
	
     
     /* show the sub-members of a group */
     gn = group_lookup(argument);
     if (gn == -1)
     {
	send_to_char("No group of that name exist.\n\r",ch);
	send_to_char(
	    "Type 'groups all' or 'info all' for a full listing.\n\r",ch);
	return;
     }

     for (sn = 0; sn < MAX_IN_GROUP; sn++)
     {
	if (group_table[gn].spells[sn] == NULL)
	    break;
	sprintf(buf,"%-20s ",group_table[gn].spells[sn]);
	send_to_char(buf,ch);
	if (++col % 3 == 0)
	    send_to_char("\n\r",ch);
     }
    if ( col % 3 != 0 )
        send_to_char( "\n\r", ch );
}

/* checks for skill improvement */
void check_improve( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
    int chance;
    char buf[100];

    if (IS_NPC(ch))
	return;

    if (ch->level < skill_table[sn].skill_level[ch->class]
    ||  skill_table[sn].rating[ch->class] == 0
    ||  ch->pcdata->learned[sn] == 0
    ||  ch->pcdata->learned[sn] == 100)
	return;  /* skill is not known */ 

    /* check to see if the character has a chance to learn */
    chance = 10 * int_app[get_curr_stat(ch,STAT_INT)].learn;
    chance /= (		multiplier
		*	skill_table[sn].rating[ch->class] 
		*	4);
    chance += ch->level;

    if (number_range(1,1000) > chance)
	return;

    /* now that the character has a CHANCE to learn, see if they really have */	

    if (success)
    {
	chance = URANGE(5,100 - ch->pcdata->learned[sn], 95);
	if (number_percent() < chance)
	{
	    sprintf(buf,"You have become better at %s!\n\r",
		    skill_table[sn].name);
	    send_to_char(buf,ch);
	    ch->pcdata->learned[sn]++;
	    gain_exp(ch,2 * skill_table[sn].rating[ch->class]);
	}
    }

    else
    {
	chance = URANGE(5,ch->pcdata->learned[sn]/2,30);
	if (number_percent() < chance)
	{
	    sprintf(buf,
		"You learn from your mistakes, and your %s skill improves.\n\r",
		skill_table[sn].name);
	    send_to_char(buf,ch);
	    ch->pcdata->learned[sn] += number_range(1,3);
	    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn],100);
	    gain_exp(ch,2 * skill_table[sn].rating[ch->class]);
	}
    }
}

/* returns a group index number given the name */
int group_lookup( const char *name )
{
    int gn;
 
    for ( gn = 0; gn < MAX_GROUP; gn++ )
    {
        if ( group_table[gn].name == NULL )
            break;
        if ( LOWER(name[0]) == LOWER(group_table[gn].name[0])
        &&   !str_prefix( name, group_table[gn].name ) )
            return gn;
    }
 
    return -1;
}

/* recursively adds a group given its number -- uses group_add */
void gn_add( CHAR_DATA *ch, int gn)
{
    int i;
    
    ch->pcdata->group_known[gn] = TRUE;
    for ( i = 0; i < MAX_IN_GROUP; i++)
    {
        if (group_table[gn].spells[i] == NULL)
            break;
        group_add(ch,group_table[gn].spells[i],FALSE);
    }
}

/* recusively removes a group given its number -- uses group_remove */
void gn_remove( CHAR_DATA *ch, int gn)
{
    int i;

    ch->pcdata->group_known[gn] = FALSE;

    for ( i = 0; i < MAX_IN_GROUP; i ++)
    {
	if (group_table[gn].spells[i] == NULL)
	    break;
	group_remove(ch,group_table[gn].spells[i]);
    }
}
	
/* use for processing a skill or group for addition  */
void group_add( CHAR_DATA *ch, const char *name, bool deduct)
{
    int sn,gn;

    if (IS_NPC(ch)) /* NPCs do not have skills */
	return;

    sn = skill_lookup(name);

    if (sn != -1)
    {
	if (ch->pcdata->learned[sn] == 0) /* i.e. not known */
	{
	    ch->pcdata->learned[sn] = 1;
	    if (deduct)
	   	ch->pcdata->points += skill_table[sn].rating[ch->class]; 
	}
	return;
    }
	
    /* now check groups */

    gn = group_lookup(name);

    if (gn != -1)
    {
	if (ch->pcdata->group_known[gn] == FALSE)  
	{
	    ch->pcdata->group_known[gn] = TRUE;
	    if (deduct)
		ch->pcdata->points += group_table[gn].rating[ch->class];
	}
	gn_add(ch,gn); /* make sure all skills in the group are known */
    }
}

/* used for processing a skill or group for deletion -- no points back! */

void group_remove(CHAR_DATA *ch, const char *name)
{
    int sn, gn;
    
     sn = skill_lookup(name);

    if (sn != -1)
    {
	ch->pcdata->learned[sn] = 0;
	return;
    }
 
    /* now check groups */
 
    gn = group_lookup(name);
 
    if (gn != -1 && ch->pcdata->group_known[gn] == TRUE)
    {
	ch->pcdata->group_known[gn] = FALSE;
	gn_remove(ch,gn);  /* be sure to call gn_add on all remaining groups */
    }
}

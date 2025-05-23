/**************************************************************************
 * SEGROMv1 was written and concieved by Eclipse<Eclipse@bud.indirect.com *
 * Soulcrusher <soul@pcix.com> and Gravestone <bones@voicenet.com> all    *
 * rights are reserved.  This is based on the original work of the DIKU   *
 * MERC coding team and Russ Taylor for the ROM2.3 code base.             *
 **************************************************************************/


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* for bzero() */
#include <time.h>
#include "merc.h"
#pragma GCC diagnostic ignored "-Wformat-overflow"
#include "interp.h"


bool	check_social	args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );

CMD_SECOND_CHAR *cmd_table_index[CMDTABLE_FIRST_HASH];
SOCIAL_SECOND_CHAR *social_table_index[SOCIALTABLE_FIRST_HASH];



/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2



/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{
    /*
	* Common movement commands.
	*/
    { "north",		do_north,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NEVER,  0 },
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NEVER,  0 },
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NEVER,  0 },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NEVER,  0 },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NEVER,  0 },
    { "northeast",	do_northeast,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "ne",		do_northeast,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "northwest",	do_northwest,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "nw",		do_northwest,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "southeast",	do_southeast,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "se",		do_southeast,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "southwest",	do_southwest,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "sw",     	do_southwest,	POS_STANDING,    0,  LOG_NEVER,  0 },
    { "put",            do_put,         POS_RESTING,     0,  LOG_NORMAL, 1 },
    { "run",		do_run,  	POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "astral walk",    do_astral_walk, POS_RESTING,    15,  LOG_NORMAL, 0 },
    { "project",        do_project,     POS_RESTING,    15,  LOG_NORMAL, 0 },
    { "shift",          do_shift,       POS_RESTING,    15,  LOG_NORMAL, 0 },
    { "enter", 		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, 0 },
    { "ride",           do_ride,        POS_STANDING,    0,  LOG_NORMAL, 0 },
    { "mount",          do_ride,        POS_STANDING,    0,  LOG_NORMAL, 0 },
       /*
	* Common other commands.
	* Placed here so one and two letter abbreviations work.
	*/
    { "at",		do_at,        	POS_DEAD,       L7,  LOG_NORMAL, 1 },
    { "buy",		do_buy,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "cc",		do_castle,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "-",		do_castle,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "castle",		do_castle,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "cgos",           do_cgos,        POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "channels",  	do_channels,  	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "donate",     	do_donate,    	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "exchange",       do_exchange,    POS_RESTING,     0,  LOG_ALWAYS, 1 },
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "goto",       	do_goto,      	POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "hit",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "ignore",         do_ignore,      POS_DEAD,        0,  LOG_ALWAYS, 1 },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "list",           do_list,        POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "listen",         do_listen,      POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "music",     	do_music,   	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "order",		do_order,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "practice",  	do_practice,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "pkill",		do_pkill,	POS_SLEEPING,	 0,  LOG_NEVER, 1 },
    { "repair",		do_repair,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "rest",		do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "sit",		do_sit,		POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "sockets",   	do_sockets,	POS_DEAD,       L7,  LOG_NORMAL, 1 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "secondary",      do_secondary,   POS_RESTING,     0,  LOG_NORMAL, 1 },
    { "tell",		do_tell,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "telekinesis",    do_telekinesis, POS_RESTING,    15,  LOG_NORMAL, 0 },
    { "tk",             do_telekinesis, POS_RESTING,    15,  LOG_NORMAL, 0 },
    { "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "wizhelp",	do_wizhelp,	POS_DEAD,	IM,  LOG_NORMAL, 1 },

	  /*
	* Informational commands.
	*/
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "changes",	do_changes,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "clairvoyance",   do_clairvoyance,POS_RESTING,    15,  LOG_NORMAL, 0 },
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "concoct",	do_concoct,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "count",		do_count,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "color",          do_color,       POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "danger",	        do_danger_sense, POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
/*  { "groups",		do_groups,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 }, */
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "info",           do_info,        POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "lore",           do_lore,        POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "motd",		do_motd,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "news",		do_news,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "read",		do_read,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "report",		do_report,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "rules",		do_rules,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "affect",		do_affect,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "attribute",	do_attribute,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "scan",		do_scan, 	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "skills",		do_skills,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "spells",		do_spells,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "story",		do_story,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "whois",		do_whois,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "wizlist",	do_wizlost,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "worth",		do_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },

    /*
	* Configuration commands.
	*/
    { "alias",		do_alias,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "arrive",		do_arrive,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "autolist",	do_autolist,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "autoassist",	do_autoassist,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autoexit",	do_autoexit,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autogold",	do_autogold,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autoloot",	do_autoloot,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autosac",	do_autosac,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "autosplit",	do_autosplit,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "afk",		do_afk,		POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "brief",		do_brief,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "color",	        do_color,	POS_DEAD,        0,  LOG_NORMAL, 1 },
/*    { "combine",	do_combine,	POS_DEAD,        0,  LOG_NORMAL, 1 },*/
    { "compact",	do_compact,	POS_DEAD,        0,  LOG_NORMAL, 1 },
	  { "damagenumbers",	do_damagenumbers,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "depart",		do_depart,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "delet",		do_delet,	POS_DEAD,	 0,  LOG_ALWAYS, 0 },
    { "delete",		do_delete,	POS_DEAD,	 0,  LOG_NEVER, 1 },
    { "maxloads",       do_lst_maxload, POS_DEAD,       L2,  LOG_NORMAL, 1 },
    { "nofollow",	do_nofollow,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "noloot",		do_noloot,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "nosummon",	do_nosummon,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "outfit",		do_outfit,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER,  1 },
    { "prompt",		do_prompt,	POS_DEAD,        0,  LOG_NORMAL, 1 },
    { "scroll",		do_scroll,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "title",		do_title,	POS_DEAD,	 3,  LOG_NORMAL, 1 },
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 1 },

    /*
	* Communication commands.
	*/
/*  { "board",		do_board,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 }, */
    { "beep",		do_beep,	POS_DEAD,	60,  LOG_NORMAL, 1 },
    { "deaf",		do_deaf,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { ".",		do_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 0 },
    { "gossip",		do_gossip,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "music",		do_music,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "note",		do_note,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "question",	do_question,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "quiet",		do_quiet,	POS_SLEEPING, 	 0,  LOG_NORMAL, 1 },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL, 0 },
    { "shout",		do_shout,	POS_RESTING,	 3,  LOG_NORMAL, 1 },
    { "yell",		do_yell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },

    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "brew",	        do_brew,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "climb",          do_climb,       POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "crawl",		do_crawl,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "flip",		do_flip,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "jump",		do_jump,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "convert",	do_convert,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "deposit",	do_deposit,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "withdraw",	do_withdraw,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "balance",	do_balance,	POS_RESTING,	 0,  LOG_ALWAYS, 1 },
    { "heal",		do_heal,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "list",		do_list,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "move",		do_move,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "pull",		do_pull,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "push",		do_push,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
/*    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL, 1 },*/
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sell",		do_sell,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "scribe",		do_scribe,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "shoot",		do_shoot,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "turn",		do_turn,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "junk",           do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, 0 },
    { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "value",		do_value,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL, 1 },

    /*
	* Combat commands.
	*/
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "bash",		do_bash,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL, 0 },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "blinding",	do_blinding_fists,POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "confuse",	do_confuse,	POS_FIGHTING,	15,  LOG_NORMAL, 0 },
    { "crane",		do_crane_dance,	POS_STANDING,	 5,  LOG_NORMAL, 0 },
    { "doorbash",       do_doorbash,    POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "dirt",		do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "dismount",       do_dismount,    POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "ego whip",       do_ego_whip,    POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "fists",		do_fists_of_fury,POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "mindblast",      do_mindblast,   POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "murde",		do_murde,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "murder",		do_murder,	POS_FIGHTING,	 0,  LOG_ALWAYS, 1 },
    { "nerve",		do_nerve_damage,POS_FIGHTING,	 0,  LOG_ALWAYS, 1 },
    { "nightmare",      do_nightmare,   POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "pyrotechnics",   do_pyrotechnics,POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "smite",          do_smite,       POS_FIGHTING,    0,  LOG_NORMAL, 1 },
    { "stunning",	do_stunning_blow, POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "torment",    	do_torment,     POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "topten",         do_topten,      POS_DEAD,        0,  LOG_NORMAL, 0 },
    { "trip",		do_trip,	POS_FIGHTING,    0,  LOG_NORMAL, 1 },

    /*
	* Miscellaneous commands.
	*/
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "aquest",		do_quest,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "join",		do_join,	POS_STANDING,	 0,  LOG_ALWAYS, 1 },
    { "gain",		do_gain,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "gainlist",       do_gainlist,    POS_SLEEPING,    5,  LOG_NORMAL, 1 },
    { "teachlist",      do_teachlist,   POS_SLEEPING,    5,  LOG_NORMAL, 1 },
    { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "leveling",   	do_leveling,   	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "groups",		do_groups,	POS_SLEEPING,    0,  LOG_NORMAL, 1 },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "iron",		do_iron_skin,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "levitate",	do_levitate,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "mindbar",    	do_mindbar,     POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "practice",	do_practice,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "psionic",    	do_psionic_armor,POS_FIGHTING,  15,  LOG_NORMAL, 0 },
    { "psychic",    	do_psychic_shield,POS_STANDING, 15,  LOG_NORMAL, 0 },
    { "shove",       	do_shove,       POS_STANDING,    3,  LOG_NORMAL, 1 },
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL, 0 },
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL, 1 },
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 1 },
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL, 0 },
    { "roll",		do_roll,	POS_DEAD,	 0,  LOG_ALWAYS, 0 },
    { "save",		do_save,	POS_DEAD,	 0,  LOG_ALWAYS, 1 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "stealth",	do_stealth,	POS_STANDING,	 0,  LOG_NORMAL, 1 },
    { "steel",		do_steel_fist,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "search",         do_search,      POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL, 1 },
    { "track",          do_track,       POS_STANDING,    0,  LOG_NORMAL, 1 },
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1 },
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL, 1 },



    /*
	* Immortal commands.
	*/
    { "advance",	do_advance,	POS_DEAD,	L4,  LOG_ALWAYS, 1 },
    { "allow",          do_allow,       POS_DEAD,       L3,  LOG_ALWAYS, 1 },
/*  { "areaload",       do_areaload,    POS_DEAD,       L4,  LOG_ALWAYS, 1 }, */
/*  { "areasave",       do_areasave,    POS_DEAD,       L4,  LOG_ALWAYS, 1 }, */
    { "backup",					show_backup,		POS_DEAD,				L1,  LOG_ALWAYS, 1 },
    { "ban",            do_ban,         POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "cloak",          do_cloak,       POS_DEAD,       L8,  LOG_ALWAYS, 1 },
    { "clone",          do_clone,       POS_DEAD,       L5,  LOG_ALWAYS, 1 },
    { "deny",           do_deny,        POS_DEAD,       L3,  LOG_ALWAYS, 1 },
/*    { "disconnect",     do_disconnect,  POS_DEAD,       L3,  LOG_ALWAYS, 1 },*/
    { "dns",            do_dns,         POS_DEAD,       ML,  LOG_ALWAYS, 1 },
    { "dump",		do_disconnect,	POS_DEAD,	L3,  LOG_ALWAYS, 1 },
    { "dump_exits",	do_dump_exits,  POS_DEAD,	ML,  LOG_ALWAYS, 1 },
    { "echo",           do_recho,       POS_DEAD,       L6,  LOG_ALWAYS, 1 },
/*    { "edit",           do_edit,        POS_DEAD,       L5,  LOG_ALWAYS, 1 },*/
    { "explode",        do_explode,     POS_DEAD,       L7,  LOG_NORMAL, 1 },
/*    { "finger",         do_finger,      POS_DEAD,       ML,  LOG_NORMAL, 1 },*/
    { "fling",          do_bowl,        POS_DEAD,       L5,  LOG_NORMAL, 1 },
    { "force",          do_force,       POS_DEAD,       L7,  LOG_ALWAYS, 1 },
    { "fsave",          do_forcesave,   POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "freeze",         do_freeze,      POS_DEAD,       L6,  LOG_ALWAYS, 1 },
    { "gather",         do_gather,      POS_DEAD,       L5,  LOG_ALWAYS, 1 },
    { "gecho",          do_echo,        POS_DEAD,       L4,  LOG_ALWAYS, 1 },
    { "gkick",          do_gkick,       POS_DEAD,       L1,  LOG_NORMAL, 1 },
    { "godtalk",        do_godtalk,     POS_DEAD,       L1,  LOG_NORMAL, 1 },
    { "gwhere",         do_gwhere,      POS_DEAD,       L8,  LOG_ALWAYS, 1 },
    { "hero",           do_hero,        POS_DEAD,       HE,  LOG_NORMAL, 1 },
    { "holylight",      do_holylight,   POS_DEAD,      L10,  LOG_NORMAL, 1 },
    { "hpardon",        do_hpardon,     POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "immtalk",        do_immtalk,     POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "imotd",          do_imotd,       POS_DEAD,       IM,  LOG_NORMAL, 1 },
    { "invis",          do_invis,       POS_DEAD,       L8,  LOG_NORMAL, 0 },
    { "itrans",         do_itrans,      POS_DEAD,       L2,  LOG_ALWAYS, 1 },
    { "jail",           do_jail,        POS_DEAD,       L8,  LOG_ALWAYS, 1 },
    { "ksock",          do_ksock,       POS_DEAD,       L6,  LOG_NORMAL, 1 },
    { "load",           do_load,        POS_DEAD,       L5,  LOG_ALWAYS, 1 },
    { "log",            do_log,         POS_DEAD,       L2,  LOG_ALWAYS, 1 },
    { "memory",         do_memory,      POS_DEAD,       L5,  LOG_NORMAL, 1 },
    { "mwhere",         do_mwhere,      POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "newcorpse",      do_newcorpse,   POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "newlock",        do_newlock,     POS_DEAD,       L4,  LOG_ALWAYS, 1 },
    { "nochannels",     do_nochannels,  POS_DEAD,       L7,  LOG_ALWAYS, 1 },
    { "noemote",        do_noemote,     POS_DEAD,       L6,  LOG_ALWAYS, 1 },
    { "nonote",         do_nonote,      POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "noshout",        do_noshout,     POS_DEAD,       L6,  LOG_ALWAYS, 1 },
    { "notell",         do_notell,      POS_DEAD,       L5,  LOG_NORMAL, 1 },
    { "notitle",        do_notitle,     POS_DEAD,       L5,  LOG_NORMAL, 1 },
/*    { "offense",        do_offense,     POS_DEAD,       L1,  LOG_NORMAL, 1 },*/
    { "owhere",         do_owhere,      POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "pardon",         do_pardon,      POS_DEAD,       L4,  LOG_ALWAYS, 1 },
    { "peace",          do_peace,       POS_DEAD,       L6,  LOG_NORMAL, 1 },
    { "pecho",          do_pecho,       POS_DEAD,       L6,  LOG_ALWAYS, 1 },
    { "poofin",         do_bamfin,      POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "poofout",        do_bamfout,     POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "pstat",          do_pstat,       POS_DEAD,       L1,  LOG_ALWAYS, 1 },
    { "purge",          do_purge,       POS_DEAD,       L4,  LOG_ALWAYS, 1 },
    { "qflag",          set_questflag,  POS_DEAD,       L8,  LOG_ALWAYS, 1 },
    { "reboot",         do_reboot,      POS_DEAD,       L2,  LOG_ALWAYS, 1 },
    { "restore",        do_restore,     POS_DEAD,       L4,  LOG_ALWAYS, 1 },
/*  {"restorechar",    do_restorechar, POS_DEAD,       L3,  LOG_ALWAYS, 1 },*/
    { "return",         do_return,      POS_DEAD,       L6,  LOG_NORMAL, 1 },
    { "rwhere",         do_rwhere,      POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "set",            do_set,         POS_DEAD,       L4,  LOG_ALWAYS, 1 },
    { "showhunt",       do_showhunt,    POS_DEAD,       L8,  LOG_NORMAL, 1 },
    { "shutdown",       do_shutdown,    POS_DEAD,       ML,  LOG_ALWAYS, 1 },
	  { "component",     do_component_update,    POS_DEAD,       L1,  LOG_ALWAYS, 1 },
    { "sla",            do_sla,         POS_DEAD,       L4,  LOG_NORMAL, 0 },
    { "slay",           do_slay,        POS_DEAD,       L4,  LOG_ALWAYS, 1 },
    { "snoop",          do_snoop,       POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "stat",           do_stat,        POS_DEAD,       L7,  LOG_NORMAL, 1 },
    { "string",         do_string,      POS_DEAD,       L7,  LOG_ALWAYS, 1 },
    { "swedish",        do_swedish,     POS_DEAD,       L3,  LOG_NORMAL, 1 },
    { "switch",         do_switch,      POS_DEAD,       L6,  LOG_ALWAYS, 1 },
    { "teleport",       do_transfer,    POS_DEAD,       L7,  LOG_ALWAYS, 0 },
    { "transfer",       do_transfer,    POS_DEAD,       L7,  LOG_ALWAYS, 1 },
    { "trust",		do_trust,	POS_DEAD,	ML,  LOG_ALWAYS, 1 },
/*    { "tset",           do_tset,        POS_DEAD,       L8,  LOG_NORMAL,1},*/
    { "undeny",         do_undeny,      POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "vnum",		do_vnum,	POS_DEAD,	L5,  LOG_NORMAL, 1 },
    { "warn",		do_warn,	POS_DEAD,	L8,  LOG_ALWAYS, 1 },
    { "whiner",         do_whiner,      POS_DEAD,       L3,  LOG_ALWAYS, 1 },
    { "wizcheck",	do_wizcheck,	POS_DEAD,	L8,  LOG_ALWAYS, 1 },
    { "transfusion",	do_transfusion, POS_FIGHTING,   15,  LOG_NORMAL, 0 },
    { "wizinvis",	do_invis,	POS_DEAD,	60,  LOG_NORMAL, 1 },
    { "wizinfo",	do_wizinfo,	POS_DEAD,	60,  LOG_NORMAL, 1 },
    { "wizlock",        do_wizlock,     POS_DEAD,       L2,  LOG_ALWAYS, 1 },
    { "@",              do_godtalk,     POS_DEAD,       L1,  LOG_NORMAL, 0 },
/*    { "heroquest",      do_heroquest,   POS_DEAD,       50,  LOG_NORMAL, 1 },*/
/*    { "endquest",       do_endquest,    POS_DEAD,       50,  LOG_NORMAL, 1 },
    { "retrieved",      do_retrieved,   POS_DEAD,       50,  LOG_NORMAL, 1 },
    { "listclue",       do_listclue,    POS_DEAD,       50,  LOG_NORMAL, 1 },*/
    { "=",		do_hero,	POS_DEAD,	HE,  LOG_NORMAL, 0 },
    { ":",		do_immtalk,	POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "remort",         do_remort,      POS_STANDING,   54,  LOG_ALWAYS, 1 },
/* End of list. */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 0 }
};



/*
 * This procedure checks all of the special procs in the room
 * to see if any handle the command or not
 */
bool check_specials(CHAR_DATA *ch, DO_FUN *cmd, char *arg)
{
    CHAR_DATA *fch, *fch_next;

    for ( fch = ch->in_room->people; fch != NULL; fch = fch_next )
    {
        fch_next = fch->next_in_room;

        if (IS_NPC(fch) && (fch->pIndexData->spec_fun != NULL) &&
                 (*fch->pIndexData->spec_fun) ( fch, ch, cmd, arg))
            return TRUE;
    }
    return FALSE;
}

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    int counter;
    bool found;

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    if(ch->in_object)
    {
      send_to_char("You have been trapped, and can't do anything.\n\r",ch);
      return;
    }
    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
      argument = one_argument( argument, command );

    /*
     * Check for aliases
     */
    if (!IS_NPC(ch))
	for (counter=0; counter<MAX_ALIASES; counter++)
	{
	    char * ptr;
	    ptr = ch->pcdata->alias[counter].first;
	    if (  ptr != NULL )
		if ( !str_cmp(command, ptr) )
		{
		    sprintf(buf, "%s %s", ch->pcdata->alias[counter].second,
		            argument);
		    argument = one_argument( buf, command );
		    break;
		}
	}

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );

/* Blackbird: We now have indexed command tables
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name )
	&&   cmd_table[cmd].level <= trust )
	{
	    found = TRUE;
	    break;
	}
    }
*/
    cmd = cmd_tab_sn_lookup(command, trust);
    if (cmd != -1) found = TRUE;

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	    send_to_char( "Huh?\n\r", ch );
	return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_SITTING:
	    send_to_char( "Better stand up first.\n\r",ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;

	}
	return;
    }

    /*
     * Check spec proc
     */
    if (check_specials(ch, cmd_table[cmd].do_fun, argument))
	return;

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

/*    }  */
    tail_chain( );
    return;
}

int cmd_tab_sn_lookup(char *arg1, int trust) {
int len;
int index1, index2;
char index_char1,index_char2;
CMD_LIST *cmd_list_elem;

  len = strlen(arg1);
  if (len <= 0) return -1;

  if (len == 1)
  {
    index_char1 = LOWER(arg1[0]);
    if (isalpha(index_char1))
    {
      index1 = (int) (index_char1 - 'a');
      if (cmd_table_index[index1] == NULL) return -1;
      if ((cmd_table_index[index1] -> def_command != NULL) &&
          (cmd_table[cmd_table_index[index1] -> def_command -> cmd_tab_sn].level <= trust))
      {
         cmd_table_index[index1] -> def_command -> called += 1;
         return cmd_table_index[index1] -> def_command -> cmd_tab_sn;
      }
      return -1;
    }
    else
    {
      index1 = CMDTABLE_FIRST_HASH-1;
      index_char2 = arg1[0];
      index2 = index_char2 % CMDTABLE_SECOND_HASH;
      if (cmd_table_index[index1] == NULL) return -1;
      if (cmd_table_index[index1] -> command_ind[index2] == NULL) return -1;
      for (cmd_list_elem = cmd_table_index[index1] -> command_ind[index2];
           cmd_list_elem != NULL;
           cmd_list_elem = cmd_list_elem -> next)
      {
       if ((!str_prefix( arg1, cmd_list_elem -> command_name)) &&
           (cmd_table[cmd_list_elem -> cmd_tab_sn].level <= trust))
       {
          cmd_list_elem -> called += 1;
          return cmd_list_elem -> cmd_tab_sn;
       } /* IF */
      } /* FOR */
      return -1;
    } /* ELSE */
    return -1;
  } /* END LEN == 1 */
  else {
    /* LEN > 1 */
    if (!isalpha(arg1[0])) return -1;
    if (!isalpha(arg1[1])) return -1;
    index_char1 = LOWER(arg1[0]);
    index1 = (int) (index_char1 - 'a');
    index_char2 = LOWER(arg1[1]);
    index2 = index_char2 % CMDTABLE_SECOND_HASH;
    if (cmd_table_index[index1] == NULL) return -1;
    if (cmd_table_index[index1] -> command_ind[index2] == NULL) return -1;
    for (cmd_list_elem = cmd_table_index[index1] -> command_ind[index2];
         cmd_list_elem != NULL;
         cmd_list_elem = cmd_list_elem -> next) {
       if ((!str_prefix( arg1, cmd_list_elem -> command_name)) &&
           (cmd_table[cmd_list_elem -> cmd_tab_sn].level <= trust)) {
          cmd_list_elem -> called += 1;
          return cmd_list_elem -> cmd_tab_sn;
       } /* IF */
    } /* FOR */
    return -1;
  } /* ELSE */
  return -1;
}


int social_tab_sn_lookup(char *arg1, int trust) {
int len;
int index1, index2;
char index_char1,index_char2;
SOCIAL_LIST *social_list_elem;

  len = strlen(arg1);
  if (len <= 0) return -1;

  if (len == 1)
  {
    index_char1 = LOWER(arg1[0]);
    if (isalpha(index_char1))
    {
      index1 = (int) (index_char1 - 'a');
      if (social_table_index[index1] == NULL) return -1;
      if (social_table_index[index1] -> def_command != NULL)
      {
         social_table_index[index1] -> def_command -> called += 1;
         return social_table_index[index1] -> def_command -> social_tab_sn;
      }
      return -1;
    }
    else
    {
      index1 = SOCIALTABLE_FIRST_HASH-1;
      index_char2 = arg1[0];
      index2 = index_char2 % SOCIALTABLE_SECOND_HASH;
      if (social_table_index[index1] == NULL) return -1;
      if (social_table_index[index1] -> command_ind[index2] == NULL) return -1;
      for (social_list_elem = social_table_index[index1] -> command_ind[index2];
           social_list_elem != NULL;
           social_list_elem = social_list_elem -> next)
      {
       if (!str_prefix( arg1, social_list_elem -> command_name))
       {
          social_list_elem -> called += 1;
          return social_list_elem -> social_tab_sn;
       } /* IF */
      } /* FOR */
      return -1;
    } /* ELSE */
    return -1;
  } /* END LEN == 1 */
  else {
    /* LEN > 1 */
    if (!isalpha(arg1[1])) return -1;
    index_char1 = LOWER(arg1[0]);
    index1 = (int) (index_char1 - 'a');
    index_char2 = LOWER(arg1[1]);
    index2 = index_char2 % SOCIALTABLE_SECOND_HASH;
    if (social_table_index[index1] == NULL) return -1;
    if (social_table_index[index1] -> command_ind[index2] == NULL) return -1;
    for (social_list_elem = social_table_index[index1] -> command_ind[index2];
         social_list_elem != NULL;
         social_list_elem = social_list_elem -> next) {
       if (!str_prefix( arg1, social_list_elem -> command_name)) {
          social_list_elem -> called += 1;
          return social_list_elem -> social_tab_sn;
       } /* IF */
    } /* FOR */
    return -1;
  } /* ELSE */
  return -1;
}

void fill_comm_table_index() {
CMD_SECOND_CHAR *cmd_sec_char = NULL;
CMD_LIST        *cmd_list_elem = NULL;
CMD_LIST        *free_spot = NULL;

int i,j,k;
int index_char;
int index_char2;
char buf[1000];

/* New function by Blackbird, initialize first the index table */
  for (i = 0; i < CMDTABLE_FIRST_HASH; i++) {
     cmd_sec_char = alloc_perm( sizeof(*cmd_sec_char));
     for (j=0;j < CMDTABLE_SECOND_HASH; j++) cmd_sec_char -> command_ind[j] = NULL;
     cmd_sec_char -> def_command = NULL;
     if (i < CMDTABLE_FIRST_HASH - 1)
        cmd_sec_char -> ind_char = (char) 'a' + i;
     else
        cmd_sec_char -> ind_char = '?';
     cmd_table_index[i] = cmd_sec_char;
  }
 /* log_string(" INIT OK ");*/
/* Loop through the cmd_table and build the index */
  for ( i = 0; cmd_table[i].name[0] != '\0'; i++ ) {
/*    sprintf(buf, "For i: %d, handling command: %s", i, cmd_table[i].name);
    log_string(buf);*/

    index_char = LOWER(cmd_table[i].name[0]);
    if (isalpha(index_char)) {
      j = (int) (index_char - 'a');
      if ((j < 0) || (j > CMDTABLE_FIRST_HASH - 1)) {
         sprintf(buf,"Building index on command_table\n"
                     "Oops, a logic error accured for command: %s\n",
                 cmd_table[i].name);
         bug(" INITIALIZE COMMAND TABLE, INDEX OUT OF RANGE %d", j);
      }
      cmd_list_elem = alloc_perm( sizeof(*cmd_list_elem) );
      cmd_list_elem -> next = NULL;
      cmd_list_elem -> command_name = str_dup(cmd_table[i].name);
      cmd_list_elem -> called       = 0;
      cmd_list_elem -> cmd_tab_sn   = i;
      /* Deal with 1 letter commands... not yet used, but anyway.. */

      if (cmd_table[i].name[1] == '\0') {
        cmd_table_index[j] -> def_command = cmd_list_elem;
      }
      else {
        index_char2 = cmd_table[i].name[1];
        index_char2 = LOWER(index_char2);
        k = ((int) index_char2) % CMDTABLE_SECOND_HASH;
        if (cmd_table_index[j] -> command_ind[k] == NULL) {
          cmd_table_index[j] -> command_ind[k] = cmd_list_elem;
          /* We could have had a one-letter command */
          if (cmd_table_index[j] -> def_command == NULL)
             cmd_table_index[j] -> def_command = cmd_list_elem;
        }
        else {
          free_spot = cmd_table_index[j] -> command_ind[k];
          if (free_spot -> next != NULL) {
             while (free_spot -> next != NULL) {
                free_spot = free_spot -> next;
             }
          }
          free_spot -> next = cmd_list_elem;
        }
      }
    }
    else {
        /* Deal with characters unlike 'a' .. 'z' */
        index_char2 = cmd_table[i].name[0];
        k = ((int) index_char2) % CMDTABLE_SECOND_HASH;
        cmd_list_elem = alloc_perm( sizeof(*cmd_list_elem) );
        cmd_list_elem -> next = NULL;
        cmd_list_elem -> command_name = str_dup(cmd_table[i].name);
        cmd_list_elem -> called       = 0;
        cmd_list_elem -> cmd_tab_sn   = i;
        if (cmd_table_index[CMDTABLE_FIRST_HASH-1] -> command_ind[k] == NULL) {
          cmd_table_index[CMDTABLE_FIRST_HASH-1] -> command_ind[k] = cmd_list_elem;
        }
        else {
          free_spot = cmd_table_index[CMDTABLE_FIRST_HASH-1] -> command_ind[k];
          if (free_spot -> next != NULL) {
             while (free_spot -> next != NULL) free_spot = free_spot -> next;
          }
          free_spot -> next = cmd_list_elem;
        }
    } /* IF */

  } /* FOR */
} /* FILL_COMM_TABLE_INDEX */


void fill_social_table_index() {
SOCIAL_SECOND_CHAR *social_sec_char = NULL;
SOCIAL_LIST        *social_list_elem = NULL;
SOCIAL_LIST        *free_spot = NULL;

int i,j,k;
int index_char;
int index_char2;
char buf[1000];

/* New function by Blackbird, initialize first the index table */
  for (i = 0; i < SOCIALTABLE_FIRST_HASH; i++) {
     social_sec_char = alloc_perm( sizeof(*social_sec_char));
     for (j=0;j < SOCIALTABLE_SECOND_HASH; j++) social_sec_char -> command_ind[j] = NULL;
     social_sec_char -> def_command = NULL;
     if (i < SOCIALTABLE_FIRST_HASH - 1)
        social_sec_char -> ind_char = (char) 'a' + i;
     else
        social_sec_char -> ind_char = '?';
     social_table_index[i] = social_sec_char;
  }
  log_string(" INIT OK ");


/* Loop through the social_table and build the index */
  for ( i = 0; social_table[i].name[0] != '\0'; i++ ) {

    index_char = LOWER(social_table[i].name[0]);
    if (isalpha(index_char)) {
      j = (int) (index_char - 'a');
      if ((j < 0) || (j > SOCIALTABLE_FIRST_HASH - 1)) {
         sprintf(buf,"Building index on social_table\n"
                     "Oops, a logic error accured for command: %s\n",
                 social_table[i].name);
         bug(" INITIALIZE SOCIAL TABLE, INDEX OUT OF RANGE %d", j);
      }
      social_list_elem = alloc_perm( sizeof(*social_list_elem) );
      social_list_elem -> next = NULL;
      social_list_elem -> command_name    = str_dup(social_table[i].name);
      social_list_elem -> called          = 0;
      social_list_elem -> social_tab_sn   = i;
      /* Deal with 1 letter commands... not yet used, but anyway.. */

      if (social_table[i].name[1] == '\0') {
        social_table_index[j] -> def_command = social_list_elem;
      }
      else {
        index_char2 = social_table[i].name[1];
        index_char2 = LOWER(index_char2);
        k = ((int) index_char2) % SOCIALTABLE_SECOND_HASH;
        if (social_table_index[j] -> command_ind[k] == NULL) {
          social_table_index[j] -> command_ind[k] = social_list_elem;
          /* We could have had a one-letter command */
          if (social_table_index[j] -> def_command == NULL)
             social_table_index[j] -> def_command = social_list_elem;
        }
        else {
          free_spot = social_table_index[j] -> command_ind[k];
          if (free_spot -> next != NULL) {
             while (free_spot -> next != NULL) {
                free_spot = free_spot -> next;
             }
          }
          free_spot -> next = social_list_elem;
        }
      }
    }
    else {
        /* Deal with characters unlike 'a' .. 'z' */
        index_char2 = social_table[i].name[0];
        k = ((int) index_char2) % SOCIALTABLE_SECOND_HASH;
        social_list_elem = alloc_perm( sizeof(*social_list_elem) );
        social_list_elem -> next = NULL;
        social_list_elem -> command_name = str_dup(social_table[i].name);
        social_list_elem -> called       = 0;
        social_list_elem -> social_tab_sn   = i;
        if (social_table_index[SOCIALTABLE_FIRST_HASH-1] -> command_ind[k] == NULL) {
          social_table_index[SOCIALTABLE_FIRST_HASH-1] -> command_ind[k] = social_list_elem;
        }
        else {
          free_spot = social_table_index[SOCIALTABLE_FIRST_HASH-1] -> command_ind[k];
          if (free_spot -> next != NULL) {
             while (free_spot -> next != NULL) free_spot = free_spot -> next;
          }
          free_spot -> next = social_list_elem;
        }
    } /* IF */

  } /* FOR */
} /* FILL_SOCIAL_TABLE_INDEX */




bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;

/* We now have indexed social tables (Blackbird)
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }
*/

    if (command[0] == '\0') return FALSE;
    if (!(((command[0] >= 'a') && (command[0] <= 'z')) ||
         ((command[0] >= 'A') && (command[0] <= 'Z'))))
      return FALSE;

    cmd = social_tab_sn_lookup(command, 0);

    if (cmd >= 0) found = TRUE;


    if ( !found )
	return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].others_no_arg);
	act( buf, ch, NULL, victim, TO_ROOM    );
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].char_no_arg);
	act( buf,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].others_auto);
	act( buf,   ch, NULL, victim, TO_ROOM    );
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].char_auto);
	act( buf, ch, NULL, victim, TO_CHAR    );
    }
    else
    {
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].others_found);
        act( buf, ch, NULL, victim, TO_NOTVICT );
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].char_found);
	act( buf, ch, NULL, victim, TO_CHAR    );
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].vict_found);
	act( buf, ch, NULL, victim, TO_VICT    );

	if ( !IS_NPC(ch) && IS_NPC(victim)
	&&   !IS_AFFECTED(victim, AFF_CHARM)
	&&   IS_AWAKE(victim)
	&&   victim->desc == NULL)
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].others_found);
		act( buf,    victim, NULL, ch, TO_NOTVICT );
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].char_found);
		act( buf,    victim, NULL, ch, TO_CHAR    );
	sprintf(buf,"\x02\x08%s\x02\x01",social_table[cmd].vict_found);
		act( buf,    victim, NULL, ch, TO_VICT    );
		break;

	    case 9: case 10: case 11: case 12:
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
		break;
	    }
	}
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{

    if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch )
	&&   cmd_table[cmd].show)
	{
	    sprintf( buf, "%-13s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( cmd_table[cmd].level >= LEVEL_HERO
	&&   cmd_table[cmd].level <= get_trust( ch )
	&&   cmd_table[cmd].show)
	{
	    sprintf( buf, "[%d] %-12s", cmd_table[cmd].level, cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 4 == 0 )   /*6*/
		send_to_char( "\n\r", ch );
	}
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}

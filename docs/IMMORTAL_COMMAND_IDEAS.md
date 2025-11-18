# Immortal command ideas

The list below brainstorms additional tools to make moderation and world-building more fun. Levels assume the existing hierarchy (Helper < Builder < Junior Immortal < Demi < Lesser/Greater/Implementor). Use or adapt to match your game's ladder.

## Helper / Guide (low immortal)
- **notequeue**: Quick view of unanswered player notes and petitions to prioritize outreach.
- **wherehelp**: Highlight newbies without a mentor and offer a teleport prompt to join them.
- **quiethelp**: Temporarily suppress global channels for a single player while you DM them.

## Builder tools
- **roomlint**: Scan the current area for missing exits, bad resets, or unlinked rooms and present a fix-it checklist.
- **mobprobe**: Inspect a mobile's scripts, resistances, and loot table in one summary without editing the area files.
- **areasmartcopy**: Copy rooms/mobs/objects between areas (with new vnums) while preserving links and scripts.

## Justice / moderation
- **shadowwatch** (mid): Stealth follow a target and stream their commands to you without revealing presence; auto-unfollow on logout.
- **hushall** (mid): Freeze all public channels for a timed cooldown with an optional reason broadcast.
- **warn** (mid): Issue formal warnings that log to player file and notify staff on next login; escalates to auto-jail after N warnings.
- **jail** (mid/high): Move a player to a configurable jail room with restricted commands and timed auto-release.
- **ipwatch** (mid/high): Live list of IPs, connection counts, and ban status with quick ban/allow toggles.
- **altcheck** (mid/high): Correlate accounts by IP/hostname/fingerprint to flag possible multi-playing; optional soft cap per host.
- **mute** (mid): Temporarily block a player from channels and tells; logs duration and reason.
- **gag** (mid): Strip color codes and profanity from a target's outgoing text for a duration.

## Investigation / logging
- **commandlog** (mid): Toggle per-player or per-room command logging with live tail and auto-expire timers.
- **pathtrace** (mid): Record a player's room path for the next N moves to audit botting or griefing routes.
- **lootwatch** (mid): Notify when protected items are looted, dropped, or destroyed, including room and actor.
- **economy pulse** (mid): Snapshot gold/material totals, auction prices, and vendor stock to detect dupes or inflation spikes.

## World control (high)
- **repopnow**: Force a repop in a target area with a reason logged to immortals.
- **storm**: Trigger global weather events (lightning hits, fog, sandstorm) that apply light debuffs for an RP event.
- **announce**: Global styled announcements with templates (event start, downtime warning) and scheduled delivery.
- **masssummon**: Pull all players in an area or matching a filter to your location for events or emergencies.
- **phasedoor**: Teleport to a specific room by vnum across planes regardless of recall/area locks.

## Building assistance
- **check typos**: Run spellcheck on room titles/descriptions in the current area and suggest fixes.
- **colorize**: Apply palette presets to room titles/exits for visual consistency.
- **resetpreview**: Simulate the next area reset and show what mobs/objects will spawn before committing changes.

## Fun / engagement
- **motd live**: Edit and preview the MOTD or IMOTD with markdown-like formatting and emoji support before publishing.
- **raffle**: Run a timed raffle for online players with eligibility filters (level range, hours played) and tracked history.
- **minievent**: Spawn a temporary mini-event (treasure goblin, wandering merchant, trivia NPC) with expiry timers and rewards.
- **emotearea**: Send atmospheric emotes to everyone in an area (rolling thunder, festival fireworks) without spamming globals.
- **disguise**: Temporarily appear as a random NPC for RP with auto-revert and logging.
- **snapshot**: Save a player's current inventory/equipment as a template to restore after events or tests.

## Safety / resilience
- **autosave pulse**: Force immediate saves for all players in high-risk zones (e.g., before a reboot) and log durations.
- **sandbox**: Clone a copy of an area into a sandbox instance where builders can test without affecting live players.
- **crashguard**: Toggle a mode that ramps up autosave frequency and command logging when crash conditions are suspected.

## Admin / implementor
- **featureflag**: Toggle new mechanics or commands on/off per account or globally with audit trails.
- **rollback**: Restore a player's state (pfile, inventory) to a checkpoint from backups.
- **profiling**: Snapshot tick/command timings and socket counts to find performance hotspots; export to log.
- **webhook**: Send moderation alerts (bans, jail, dupe detections) to an external webhook/Discord for on-call visibility.

Each idea should come with clear help entries, logging, and access checks to match the intended staff level.

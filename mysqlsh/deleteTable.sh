#!/bin/bash

#用来清除表里所有数据

host=10.96.29.36;
user=houbin;
password=123456;
db=bleach_hb;
tables="
Activity
ArenaRank
ArenaRecord
ArenaShop
Blade
Buff
CharBase
CommandPhone
Copy
DailyShop
Gacha
MonsterRecord
QuestDoing
QuestDone
RankArena
RankBP
RoleArena
Shop
Soul
SoulFight
SoulRoom
UserMail
";

for tb in $tables
do
	sql="use ${db}; delete from ${tb};";
	mysql -u${user} -p${password} -e "${sql}";
done

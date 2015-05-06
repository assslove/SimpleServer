#!/bin/bash
# 该脚本用于导出具体的号到内网，用于验证bug或者其它使用
host=10.96.29.36;
user=pub;
password=123456;
uid=9663885658750985;
db=bleach_pub;
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
	sql="uid=${uid}";
	mysqldump -h${host} -u${user} -p${password} ${db} ${tb} -t -w${sql} >> ${uid}.sql;
done

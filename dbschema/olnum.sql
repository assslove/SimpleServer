--统计在线人数 每隔一定时间间隔收集数据
create database stat;
drop table if exists stat.t_ol_num_pid_2015;
-- 表名为t_ol_num + pid + year
create table stat.t_ol_num_1_2015 (
	sid int COMMENT 'server id',	
	record_time int COMMENT 'record time', 
	count int COMMENT 'count',  
	primary key(sid, record_time)
) ENGINE=InnoDB default charset=utf8;


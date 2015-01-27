use TEST;
drop table if exists t_user;
create table t_user (
	user_id int, 
	nick char(16),
	score int not NULL,
	primary key (user_id)
) ENGINE=InnoDB;

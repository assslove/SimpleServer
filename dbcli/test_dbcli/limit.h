#ifndef _LIMIT_H
#define _LIMIT_H

class Limit : public TableRouter 
{
	public:
		Limit(MysqlCli *mc_);
		~Limit() {}
};

#endif

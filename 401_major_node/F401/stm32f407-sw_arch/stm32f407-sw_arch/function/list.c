#include "stm32f4xx.h"
#include "wujique_log.h"
#include "list.h"

/*
���Ժ�offsetof���ܣ���ȡ�ṹ���Ա�ڽṹ���е�ƫ��

struct test  
{  
        int i;  
        int j;  
        char k;
		struct list_head list;
		char*name;
};  



void list_test_offsetof(void)
{
	    struct test temp;  
  
        wjq_log(LOG_FUN, "&temp = %p\r\n", &temp);          // temp����ʼ��ַ  
        wjq_log(LOG_FUN, "&(temp.k) = %p\r\n", &(temp.k));      // temp.k�ĵ�ַ  
        wjq_log(LOG_FUN, "&(((struct test *)0)->k) = %ld\r\n", (size_t)&((struct test *)0)->k);       //k��temp�е�ƫ����  
  		while(1);
}
*/
/*���岢��ʼ��һ������ͷ��������*/
struct list_head TestList={&TestList, &TestList};

struct test  
{  
        int i;  
        int j;  
        char k;
		struct list_head list;
		char*name;
};

struct test t1={
	.name = "tset list 1",
	}; 
struct test t2={
	.name = "tset list 2",
	}; 
struct test t3={
	.name = "tset list 3",
	}; 


void list_test(void)
{
	int res;
	
	struct test *p;

	res = list_empty(&TestList);
	if(res  == 1)
	{
		wjq_log(LOG_FUN, "list is empty\r\n");
	}
	
	list_add(&(t1.list), &TestList);

	p = list_entry(TestList.next,struct test,list);
	
	wjq_log(LOG_FUN, "name:%s\r\n", p->name);

	while(1);
	
}



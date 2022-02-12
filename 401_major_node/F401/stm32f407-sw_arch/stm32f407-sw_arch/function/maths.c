/**
 * @file            
 * @brief           ��ѧ����
 * @author          wujique
 * @date            2018��06��26�� ���ڶ�
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:      
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
       	1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱���������������뱣��WUJIQUE��Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/

#include "stm32f4xx.h"
#include "wujique_log.h"

/*

�ǵݹ��ж�һ������2�Ķ��ٴη�

*/
int log2(int value) 
{ 
  int x=0; 
  while(value>1) 
  { 
	value>>=1; 
	x++; 
  } 
  return x; 
} 

/*
	�������U16 ����ϣ������
*/
void ShellSort(u16 *pSrc, s32 Len)
{
    u32 d;//����
    s32 j, k;

    u16 temp;

    //uart_printf("shell:%d\r\n", Len);

    for(d = Len/2; d >0; d=d/2)
    {
        for(j = d; j < Len; j++)
        {
            temp = *(pSrc + j);

            for(k = j - d; (k >=0 && temp <(*(pSrc + k))); k -=d)
            {
                *(pSrc + k + d) = *(pSrc + k);
            }

            *(pSrc + k + d) = temp;
        }
    }
}

/*
	��������: 
	��������: ���ַ�����u16
	��ڲ���: 
	�� �� ֵ: ����-1û�ҵ�������Ϊ����ֵ
*/
int BinarySearch(u16 *pArray, u16 data, int front, int end)
{
    int low, high, mid;
    low = front;
    high = end;
    while(low <= high)
    {

        mid = (low + high) / 2;
        if (*(pArray + mid) < data)
        {
            low = mid + 1;
        }
        else if (*(pArray + mid) > data)
        {
            high = mid - 1;
        }
        else 
        {
            return mid;
        }
    }
    return -1;
}


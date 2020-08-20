/**
 * 2020��08��17�� 21/12/16
 * ���̼��ź�ͨ��
 */
#include	<type.h>
#include	<signal.h>

PUBLIC	void	setSignal(SIGNAL* signal, T_PF_SA_HANDLER handler, u_32 sa_mask, u_32 sa_flags, T_PF_SA_RESTORER restorer)
{
	signal->handler = handler;
	signal->sa_mask = sa_mask;
	signal->sa_flags = sa_flags;
	signal->restorer = restorer;
}
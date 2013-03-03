/*------------------------------------------------------------------------------------------------------------------
--			FUNCTION:		pow
--
--			DATE:			FEB 28, 2013
--
--			REVISIONS:		Mar, 2013
--
--			DESIGNER:		Nicholas Raposo	(Team Leader)
--			    &			Christopher Porter
--			PROGRAMMER:	    Cody Srossiter
--							Aiko Rose	
--
--			INTERFACE:		int pow(int base,int num)
--
--			RETURNS:		
--
--			NOTES:			
							 
--
----------------------------------------------------------------------------------------------------------------------*/

#include <windows.h>

int pow(int base,int num)	
{
	int sum=1;
	for(int i=0;i<num;i++)
		sum*=base;
	return sum;
}
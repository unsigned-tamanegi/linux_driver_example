#include "nsystem_util.h"

typedef unsigned long long	minor_list_t;
static minor_list_t			minor_list = (minor_list_t)0;

int
get_minor_no(void)
{
	int				i1 = 0;
	int				max = (int)sizeof(minor_list) * 8;
	int				ret = -1;

	for(i1 = 0; i1 < max; i1++) {
		if(((minor_list >> i1) & (minor_list_t)1) == 0) {
			minor_list = (((minor_list_t)1 << i1)) | minor_list;
			ret = i1;
			goto out;
		}
	}
out :
	return ret;
}

void
put_minor_no(int minor_no)
{
	minor_list = ~(((minor_list_t)1 << minor_no)) & minor_list;
}


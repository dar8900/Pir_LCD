#define SEC_IN_DAY		   86400
#define SEC_IN_HOUR 		3600
#define SEC_IN_MINUTE 	      60
#define SEC_IN_31_GG 	 2678400
#define HOUR_IN_DAY			  23
#define MINUTE_IN_HOUR 	      59
#define WHILE_LOOP_DELAY	  30

typedef struct
{
	short hour;
	short minute;
	short second;
	short day;
	short month;
	short year;
} TIME_DATE_FORMAT;

enum
{
	CHANGE_HOUR = 0,
	CHANGE_MINUTE,
	CHANGE_DAY,
	CHANGE_MONTH,
	CHANGE_YEAR,
	EXIT_CHANGE_TIME
};


void RTCInit(void);
bool ChangeTime(void);
void TimeAdjust(short Hour, short Minute, short Day, short Month, short Year);
void ShowDateTime(short Row);
void TakePresentTime(void);

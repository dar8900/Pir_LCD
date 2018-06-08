#define INVALID_BAND_VALUE 	99

enum
{
	INIT_HOUR = 0,
	INIT_MINUTE,
	END_HOUR,
	END_MINUTE,
	EXIT
};

enum
{
	VALID = 0,
	INVALID
};

typedef struct
{
	short InitHour;
	short InitMinute;
	short InitDay;
	short EndHour;
	short EndMinute;
	short EndDay;

} BAND_FORMAT;

void BandInit(void);
bool IsBandCorrect(void);
bool CheckBand(void);
void SetBandInvalid(void);
void SaveBandValues(short WichItem);
void ReadBandValues(short WichItem);
bool SetTimeBand(void);

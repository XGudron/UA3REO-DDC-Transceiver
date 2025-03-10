#ifndef WOLF_h
#define WOLF_h

typedef enum {
	TRX_MODE_LSB,
	TRX_MODE_USB,
	TRX_MODE_CW,
	TRX_MODE_NFM,
	TRX_MODE_WFM,
	TRX_MODE_AM,
	TRX_MODE_SAM_STEREO,
	TRX_MODE_SAM_LSB,
	TRX_MODE_SAM_USB,
	TRX_MODE_DIGI_L,
	TRX_MODE_DIGI_U,
	TRX_MODE_IQ,
	TRX_MODE_LOOPBACK,
	TRX_MODE_RTTY,
	TRX_MODE_DSB,
} TRX_MODE;

typedef enum {
	TRX_SELECTED_VFO_A,
	TRX_SELECTED_VFO_B,
} TRX_SELECTED_VFO;

typedef struct { // TRX information structure
	uint8_t CurrentVFO;
	uint64_t VFO_A_Frequency;
	uint64_t VFO_B_Frequency;
	uint8_t VFO_A_Mode;
	uint8_t VFO_B_Mode;
	char CAT_Answer[64];
	bool TX;
	bool Tune;
} tWOLF;

typedef struct { // Interface information structure
	bool hasDataChanges;
	char Message[64];
	char CAT_Command[64];
} tWOLFInterface;

extern tWOLF TRX_WOLF;

extern void WOLF_init(void);
extern String WOLF_modeToString(uint8_t mode);
extern void WOLF_setMessage(String message);
extern String WOLF_catCommand(String command);

#endif

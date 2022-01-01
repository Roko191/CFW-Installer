#pragma once

// Just delacaring all fuctions for later usage

static inline void _orbisCommonDialogSetMagicNumber(uint32_t* magic, const OrbisCommonDialogBaseParam* param);
static inline void _orbisCommonDialogBaseParamInit(OrbisCommonDialogBaseParam *param);
static inline void orbisMsgDialogParamInitialize(OrbisMsgDialogParam *param);
int show_dialog(int tdialog, const char * format, ...);
void init_progress_bar(const char* msg);
void end_progress_bar(void);
int init_loading_screen(const char* message);
void stop_loading_screen();
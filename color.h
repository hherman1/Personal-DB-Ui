#pragma once

#define R_COLOR_SCHEME_CLASSIC 0
#define R_COLOR_SCHEME_CLASSIC_DELETE 1
/*
#define R_COLOR_SCHEME_DEFAULT -1
#define R_COLOR_SCHEME_TEXT 0
#define R_COLOR_SCHEME_HOVERED 1
#define R_COLOR_SCHEME_SELECTED 2
#define R_COLOR_SCHEME_SELECTED_HOVERED 3
#define R_COLOR_SCHEME_SELECTED_BODY 4
#define R_COLOR_SCHEME_SELECTED_BODY_HOVERED 5
#define R_COLOR_SCHEME_DELETE 6
#define R_COLOR_SCHEME_DELETE_HOVERED 7
#define R_COLOR_SCHEME_DELETE_BODY 8
#define R_COLOR_SCHEME_DELETE_BODY_HOVERED 9
*/
#define UI_COLOR_SCHEME_MODE_BAR 10
#define UI_COLOR_SCHEME_MODE_BAR_SELECTED 11

#define R_TEXT_COLOR XT_CH_CYAN

#define R_HOVERED_TEXT_STYLE XT_CH_BOLD
#define R_HOVERED_BG_COLOR XT_BG_DEFAULT
#define R_HOVERED_TEXT_COLOR XT_CH_BLUE

#define R_SELECTED_BG_STYLE XT_BG_DEFAULT
#define R_SELECTED_BODY_BG_STYLE XT_BG_DEFAULT
#define R_SELECTED_TEXT_COLOR XT_CH_MAGENTA
#define R_SELECTED_BODY_COLOR XT_CH_MAGENTA
#define R_SELECTED_TEXT_STYLE XT_CH_UNDERLINE
#define R_SELECTED_BODY_STYLE XT_CH_NORMAL
#define R_SELECTED_HOVERED_TEXT_COLOR XT_CH_BLACK
#define R_SELECTED_HOVERED_BODY_COLOR XT_CH_MAGENTA
#define R_SELECTED_HOVERED_TEXT_STYLE XT_CH_BOLD XT_CH_UNDERLINE
#define R_SELECTED_HOVERED_BG_COLOR XT_BG_DEFAULT

#define R_DELETE_TEXT_COLOR XT_CH_RED
#define R_DELETE_BG_COLOR XT_BG_DEFAULT
#define R_DELETE_TEXT_STYLE XT_CH_BOLD XT_CH_BLINK
#define R_DELETE_BODY_STYLE XT_CH_NORMAL XT_CH_BLINK

#define DB_VARIABLE_DATA_STYLE XT_CH_BOLD XT_CH_RED XT_BG_WHITE
#define db_var_col(string) f_color(string,DB_VARIABLE_DATA_STYLE)

#define UI_MODE_BAR_TEXT_COLOR XT_CH_YELLOW
#define UI_MODE_BAR_TEXT_STYLE XT_CH_NORMAL
#define UI_MODE_BAR_BG_COLOR XT_BG_DEFAULT

#define UI_MODE_BAR_SELECTED_TEXT_STYLE XT_CH_BOLD

#define UI_DELETE_CONFIRM_COLOR XT_CH_BOLD XT_CH_RED

#define f_color(string,color) color string XT_CH_NORMAL

#define colorScheme(base,index) 

struct ColorScheme {
	char *textStyle;
	char *textColor;
	char *bgColor;
};
struct RecordColor {
	struct ColorScheme subject;
	struct ColorScheme body;
};
struct RecordColorScheme {
	struct RecordColor normal;
	struct RecordColor hovered;
	struct RecordColor selected;
	struct RecordColor selectedHovered;
};
void initScheme();
void setColor(int colorScheme);
void displayColor(struct ColorScheme colorScheme);
char *getColor(struct ColorScheme colorScheme);

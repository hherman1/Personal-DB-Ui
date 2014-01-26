#include "color.h"
#include "xterm_control.h"
#include "memory.h"

struct ColorScheme defaultColors = {
	XT_CH_NORMAL,XT_BG_DEFAULT,XT_CH_DEFAULT
};
struct RecordColorScheme scheme = {
	{
		{
			XT_CH_NORMAL,R_TEXT_COLOR,XT_BG_DEFAULT
		},
		{
			XT_CH_NORMAL,R_TEXT_COLOR,XT_BG_DEFAULT			
		}
	},
	{
		{
			R_HOVERED_TEXT_STYLE,R_HOVERED_TEXT_COLOR,R_HOVERED_BG_COLOR
		},
		{
			R_HOVERED_TEXT_STYLE,R_HOVERED_TEXT_COLOR,R_HOVERED_BG_COLOR
		}
	}
};
struct RecordColor classicSelected = {
	{
		R_SELECTED_TEXT_STYLE,R_SELECTED_TEXT_COLOR,R_SELECTED_BG_STYLE
	},
	{
		R_SELECTED_BODY_STYLE,R_SELECTED_BODY_COLOR,R_SELECTED_BODY_BG_STYLE
	}
};
struct RecordColor classicSelectedHovered = {
	{
		R_SELECTED_TEXT_STYLE,R_SELECTED_HOVERED_TEXT_COLOR,R_SELECTED_HOVERED_BG_COLOR
	},
	{
		R_SELECTED_BODY_STYLE,R_SELECTED_HOVERED_BODY_COLOR,R_SELECTED_HOVERED_BG_COLOR
	}
};

struct RecordColor classicDelete = {
	{
		R_DELETE_TEXT_STYLE,R_DELETE_TEXT_COLOR,R_DELETE_BG_COLOR
	},
	{
		R_DELETE_BODY_STYLE,R_DELETE_TEXT_COLOR,R_DELETE_BG_COLOR
	}
};

struct RecordColor classicModeBar = {
	{
		UI_MODE_BAR_TEXT_STYLE,UI_MODE_BAR_TEXT_COLOR,UI_MODE_BAR_BG_COLOR
	},
	{
		UI_MODE_BAR_SELECTED_TEXT_STYLE,UI_MODE_BAR_TEXT_COLOR,UI_MODE_BAR_BG_COLOR
	}
};
void initScheme() {
	setColor(R_COLOR_SCHEME_CLASSIC);
}
void setColor(int colorScheme) {
	if(colorScheme == R_COLOR_SCHEME_CLASSIC) {
		scheme.selected = classicSelected;
		scheme.selectedHovered = classicSelectedHovered;
	}
	else if(colorScheme == R_COLOR_SCHEME_CLASSIC_DELETE) {
		scheme.selected = classicDelete;
		scheme.selectedHovered = classicDelete;
	}
}
void displayColor(struct ColorScheme colorScheme) {
	printf(getColor(colorScheme));
}
char *getColor(struct ColorScheme colorScheme) {
	char *format = "%s%s%s";
	return saveFormatted(format,colorScheme.textStyle,colorScheme.textColor,colorScheme.bgColor);
}

/*void setColor(int colorScheme) {
	printf(getColor(colorScheme));
}

/*
char *getColor(int colorScheme) {
	char *ans;
	char *styleFormat = "%s%s%s";
	if(colorScheme == R_COLOR_SCHEME_DEFAULT) {
		ans = saveFormatted(styleFormat,XT_CH_NORMAL,XT_BG_DEFAULT,XT_CH_DEFAULT);
	} else if(colorScheme == R_COLOR_SCHEME_TEXT) {
		ans = R_TEXT_COLOR;
	} else if (colorScheme == R_COLOR_SCHEME_HOVERED) {
		ans = saveFormatted(styleFormat,getColor(R_COLOR_SCHEME_DEFAULT),R_HOVERED_BG_COLOR,R_HOVERED_TEXT_COLOR);
	}
	 else if (colorScheme == R_COLOR_SCHEME_SELECTED) {	
		ans = saveFormatted(styleFormat,R_SELECTED_TEXT_STYLE,R_SELECTED_BG_COLOR,R_SELECTED_TEXT_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_SELECTED_BODY) {		
		ans = saveFormatted(styleFormat,R_SELECTED_BODY_STYLE,R_SELECTED_BG_COLOR,R_SELECTED_BODY_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_SELECTED_HOVERED) {
		ans = saveFormatted(styleFormat,getColor(R_COLOR_SCHEME_SELECTED),R_SELECTED_HOVERED_BG_COLOR,R_SELECTED_HOVERED_TEXT_COLOR);
	}
	 else if (colorScheme == R_COLOR_SCHEME_DELETE) {
		ans = saveFormatted(styleFormat,R_DELETE_TEXT_STYLE,R_DELETE_BG_COLOR,R_DELETE_TEXT_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_DELETE_BODY) {
		ans = saveFormatted(styleFormat,R_DELETE_BODY_STYLE,R_DELETE_TEXT_COLOR,R_DELETE_BG_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_DELETE_HOVERED) {
		ans = getColor(R_COLOR_SCHEME_DELETE);
	}
	 else if (colorScheme == UI_COLOR_SCHEME_MODE_BAR) {
		ans = saveFormatted(styleFormat,UI_MODE_BAR_TEXT_STYLE,UI_MODE_BAR_TEXT_COLOR,UI_MODE_BAR_BG_COLOR);
	} else if (colorScheme == UI_COLOR_SCHEME_MODE_BAR_SELECTED) {
		ans = saveFormatted(styleFormat,UI_MODE_BAR_SELECTED_TEXT_STYLE,UI_MODE_BAR_TEXT_COLOR,UI_MODE_BAR_BG_COLOR);
	}
	return ans;
}*/
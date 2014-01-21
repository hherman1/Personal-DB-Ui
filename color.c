#include "color.h"
#include "xterm_control.h"


void setColor(int colorScheme) {

	if(colorScheme == R_COLOR_SCHEME_DEFAULT) {
		xt_par0(XT_CH_NORMAL);
		xt_par0(XT_BG_DEFAULT);
		xt_par0(XT_CH_DEFAULT);
	} else if(colorScheme == R_COLOR_SCHEME_TEXT) {
		xt_par0(R_TEXT_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_HOVERED) {
		xt_par0(R_HOVERED_BG_COLOR);
		xt_par0(R_HOVERED_TEXT_COLOR);		
	} else if (colorScheme == R_COLOR_SCHEME_SELECTED) {	

		xt_par0(R_SELECTED_TEXT_STYLE);
		xt_par0(R_SELECTED_BG_COLOR);
		xt_par0(R_SELECTED_TEXT_COLOR);

	} else if (colorScheme == R_COLOR_SCHEME_SELECTED_BODY) {		
		xt_par0(R_SELECTED_BODY_STYLE);
		xt_par0(R_SELECTED_BG_COLOR);
		xt_par0(R_SELECTED_BODY_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_SELECTED_HOVERED) {
		setColor(R_COLOR_SCHEME_SELECTED);
		xt_par0(R_SELECTED_HOVERED_TEXT_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_DELETE) {
		xt_par0(R_DELETE_TEXT_STYLE);
		xt_par0(R_DELETE_BG_COLOR);
		xt_par0(R_DELETE_TEXT_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_DELETE_BODY) {
		xt_par0(R_DELETE_BODY_STYLE);		
		xt_par0(R_DELETE_TEXT_COLOR);
		xt_par0(R_DELETE_BG_COLOR);
	} else if (colorScheme == R_COLOR_SCHEME_DELETE_HOVERED) {
		setColor(R_COLOR_SCHEME_DELETE);
	} else if (colorScheme == UI_COLOR_SCHEME_MODE_BAR) {
		xt_par0(UI_MODE_BAR_TEXT_STYLE);
		xt_par0(UI_MODE_BAR_TEXT_COLOR);
		xt_par0(UI_MODE_BAR_BG_COLOR);
	} else if (colorScheme == UI_COLOR_SCHEME_MODE_BAR_SELECTED) {
		xt_par0(UI_MODE_BAR_SELECTED_TEXT_STYLE);
		xt_par0(UI_MODE_BAR_TEXT_COLOR);
		xt_par0(UI_MODE_BAR_BG_COLOR);
	}
}
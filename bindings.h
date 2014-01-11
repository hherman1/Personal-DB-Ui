#define KEY_MODE_ESCAPE(c) c == KEY_F10
#define KEY_MODE_RECORDS(c) c == KEY_F2
#define KEY_MODE_ADD(c) c== 'a' || c == KEY_F3
#define KEY_MODE_SEARCH(c) c == 's' || c == KEY_F5
#define KEY_MODE_EDIT(c) c =='e' || c == KEY_F4
#define KEY_MODE_DELETE(c) c == 'd' || c == KEY_F6
#define KEY_MODE_DELETE_CONFIRM(c) c == 'y' || c == 'Y' || c == KEY_ENTER
#define KEY_MODE_DELETE_DENY(c) c == 'n' || c == 'N' || c == KEY_MODE_ESCAPE(c)

#define KEY_QUIT KEY_F8

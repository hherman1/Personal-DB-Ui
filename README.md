What works:
add, edit, delete, search, scrolling records view

Whats missing:
dynamic UI (outside of record pane/message box)
synchronized buffers

What you wont be able to figure out in the program: (until
later revision)
add is 'a'
delete is 'd'
edit is 'e'
search is 's'
select is ENTER

details:
-make and runmyui2
-up and down arrow keys to traverse record in display.
- the record display will scroll up and down when theres 
  a lot of records
-enter over a record to display body
-'e' over a record to edit. enter during edit to complete
-'s' to search. type to see text appear in search area
-'a'to record. area is at bottom. Start typing tosee text. 
  enter to finish new subject,enter again to finish new body and returns to view

things to fix:
-deleting from a search view -- Memory leak in attempted
synchronization
-can't press q in editing or adding or u'll quit since q is also for quit
  forgot to change that
-backspace bug during adding

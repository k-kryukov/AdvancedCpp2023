# What are you looking at?

This application provides you with both server/client of ToDoListService
With the client application you may register/login with username/password
You are able to create notes with various options


## Project architecture

### Client


### Server

- create_user(username, pswd_hash)
- remove_user(username, pswd_hash)
- add_note(username, pswd_hash, note) -> id
- remove_note(username, pswd_hash, id)
- get_notes(username, pswd_hash)
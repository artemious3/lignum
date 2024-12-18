
# Lignum TODO list

## Database 

- [+] Add `removePerson` function
     
For the sake of simplicity it should remove only leaf nodes.

- [ ] Check DB for validity in `Load`


## UI

- [ ] Disallow adding partners, ancestors and descendants to secondary person.

In the future *expanded mode* will be introduced with multiple clusters.

- [ ] Add `removePerson` action.


## Misc

- [ ] Exclude assumption that second parent is nonempty

- [ ] Replace all functions call that assume id existing in DB 

In many components of Lignum it is assumed that id 1 exists in DB. After
introducing remove functions this assumption can hold wrong. 

- [ ] OPTIMIZATION

All the Renderer components seem really heavy. It is especially vulnerable
considering that it runs every time the tree is updated.
They should be stress-tested on large trees. 

- [ ] Catch all exceptions


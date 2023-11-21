## Background and TODOs:

- [ ] db should store `(key, (value, timestamp))` instead of `(key, value)`.

- [ ] update memo would store `(key, (timestamp, counter))`.
    - `counter` is the number of obsolete versions of this item in the index.

- [ ] on insert:
    - increment `global_timestamp` by 1.
    - add `(key, (value, global_timestamp))` to memtable

- [ ] on delete:
    - increment `global_timestamp` by 1.
    - if key exists in UM:
        - `e = um(key)`
        - `e.timestamp <- global_timestamp`
        - `e.counter <- e.counter + 1`
    - if key does not exist in UM:
        - add item to the UM with `(key, global_timestamp, 1)`

- [ ] on update (and delete):
    - increment `global_timestamp` by 1
    - if key exists in UM:
        - `e = um(key)`
        - `e.timestamp <- global_timestamp`
        - `e.counter <- e.counter + 1`
    - if key does not exist in UM:
        - add item to the UM with `(key, global_timestamp, 1)`
        - add `(key, (value, global_timestamp))` to memtable

- [ ] when searching:
    - TODO

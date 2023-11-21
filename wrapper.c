// Compilation, assuming leveldb contains the sources
// g++ wrapper.c -L ./leveldb/build/ -I leveldb/include/ -lleveldb -lpthread

#include <leveldb/c.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addData(leveldb_t* db, leveldb_writeoptions_t* woptions) {
  char key[100], value[100];
  char* err = NULL;  // Error pointer

  printf("Enter key: ");
  scanf("%s", key);
  printf("Enter value: ");
  scanf("%s", value);

  // Perform the database write operation
  leveldb_put(db, woptions, key, strlen(key), value, strlen(value), &err);

  // Check if there was an error
  if (err != NULL) {
    fprintf(stderr, "Error adding data: %s\n", err);
    free(err);  // Don't forget to free the error message
  } else {
    printf("Data added successfully.\n");
  }
}

void viewData(leveldb_t* db, leveldb_readoptions_t* roptions) {
  char key[100];
  printf("Enter key to retrieve: ");
  scanf("%s", key);

  size_t read_len;
  char* read = leveldb_get(db, roptions, key, strlen(key), &read_len, NULL);
  if (read != NULL) {
    printf("Value: %.*s\n", (int)read_len, read);
    free(read);
  } else {
    printf("Key not found.\n");
  }
}

void deleteData(leveldb_t *db, leveldb_writeoptions_t *woptions) {
  char key[100];
  printf("Enter key to delete: ");
  scanf("%s", key);

  char *err = NULL;
  leveldb_delete(db, woptions, key, strlen(key), &err);
  if (err != NULL) {
    fprintf(stderr, "Error deleting data: %s\n", err);
    free(err);
  } else {
    printf("Data deleted successfully.\n");
  }
}

void viewAll(leveldb_t* db, leveldb_readoptions_t* roptions) {
  leveldb_iterator_t* it = leveldb_create_iterator(db, roptions);

  for (leveldb_iter_seek_to_first(it); leveldb_iter_valid(it);
       leveldb_iter_next(it)) {
    size_t key_len, value_len;
    const char* key = leveldb_iter_key(it, &key_len);
    const char* value = leveldb_iter_value(it, &value_len);
    printf("Key: %.*s, Value: %.*s\n", (int)key_len, key, (int)value_len,
           value);
  }

  leveldb_iter_destroy(it);
}

int main(int argc, char** argv) {
  leveldb_t* db;
  leveldb_options_t* options = leveldb_options_create();
  leveldb_options_set_create_if_missing(options, 1);
  char* err = NULL;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <path_to_db>\n", argv[0]);
    return -1;
  }

  db = leveldb_open(options, argv[1], &err);
  if (err != NULL) {
    fprintf(stderr, "Unable to open/create database: %s\n", err);
    return -1;
  }

  leveldb_writeoptions_t* woptions = leveldb_writeoptions_create();
  leveldb_readoptions_t* roptions = leveldb_readoptions_create();

  while (1) {
    printf(
        "\n1. Add Data\n2. View Data\n3. View All\n4. Delete\n5. Exit\nEnter your "
        "choice: ");
    int choice;
    scanf("%d", &choice);

    switch (choice) {
      case 1:
        addData(db, woptions);
        break;
      case 2:
        viewData(db, roptions);
        break;
      case 3:
        viewAll(db, roptions);
        break;
      case 4:
        deleteData(db, woptions);
        break;
      case 5:
        leveldb_close(db);
        return 0;
      default:
        printf("Invalid choice.\n");
    }
  }

  leveldb_close(db);
  leveldb_options_destroy(options);
  leveldb_writeoptions_destroy(woptions);
  leveldb_readoptions_destroy(roptions);

  return 0;
}

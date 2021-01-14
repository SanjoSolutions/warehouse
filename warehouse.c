#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define GOODS_FILE_PATH "warehouse.csv"

struct GoodsPosition {
  char* name;
  int amount;
};

struct GoodsPosition* createGoodsPosition(char* name, int amount) {
  struct GoodsPosition* goodsPosition = malloc(sizeof(struct GoodsPosition));
  goodsPosition->name = name;
  goodsPosition->amount = amount;
  return goodsPosition;
}

struct ListItem {
  void* value;
  void* nextListItem;
};

struct ListItem* createListItem(void* value, struct ListItem* nextListItem) {
  struct ListItem* listItem = malloc(sizeof(struct ListItem));
  listItem->value = value;
  listItem->nextListItem = NULL;
  return listItem;
}

void appendListItem(struct ListItem* firstListItem, struct ListItem* listItem) {
  if (firstListItem) {
    struct ListItem* lastListItem = firstListItem;
    while (lastListItem->nextListItem) {
      lastListItem = lastListItem->nextListItem;
    }
    lastListItem->nextListItem = listItem;
  } else {
    firstListItem = listItem;
  }
}

struct ListItem* createAndAppendGoodsPositionListItem(struct ListItem* firstListItem, char* name, int amount) {
  struct GoodsPosition* goodsPosition = createGoodsPosition(name, 0);
  struct ListItem* listItem = createListItem(goodsPosition, NULL);
  appendListItem(firstListItem, listItem);
  return listItem;
}

struct ListItem* findGoodsPositionListItemByName(struct ListItem* firstListItem, char* name) {
  struct ListItem* listItem = firstListItem;
  do {
    if (strcmp(((struct GoodsPosition*)listItem->value)->name, name) == 0) {
      return listItem;
    }
    listItem = listItem->nextListItem;
  } while (listItem);
  return NULL;
}

struct ListItem* readGoods() {
  FILE* file = fopen(GOODS_FILE_PATH, "r");
  struct ListItem* goods = NULL;
  struct ListItem* lastListItem = NULL;
  int character = fgetc(file);
  int fieldIndex = 0;
  char string[2048];
  int stringIndex = 0;
  char* name = NULL;
  int amount = 0;
  while (character != EOF) {
    if (character == ',' || character == '\n') {
      string[stringIndex] = '\0';
      stringIndex++;
      if (fieldIndex == 0) {
        int stringLength = stringIndex;
        name = malloc(stringLength*  sizeof(char));
        strncpy(name, string, stringLength);
      } else if (fieldIndex == 1) {
        amount = atoi(string);
      }
      fieldIndex++;
      stringIndex = 0;

      if (character == '\n') {
        struct GoodsPosition* goodsPosition = malloc(sizeof(struct GoodsPosition));
        goodsPosition->name = name;
        goodsPosition->amount = amount;
        struct ListItem* listItem = malloc(sizeof(struct ListItem));
        listItem->value = goodsPosition;
        listItem->nextListItem = NULL;
        if (lastListItem) {
          lastListItem->nextListItem = listItem;
        }
        else {
          goods = listItem;
        }
        lastListItem = listItem;
        fieldIndex = 0;
        name = NULL;
        amount = 0;
      }
    }
    else {
      string[stringIndex] = character;
      stringIndex++;
    }
    character = fgetc(file);
  }
  return goods;
};

void writeGoodsPositionListItem(FILE* file, struct ListItem* listItem) {
  struct GoodsPosition* goodPosition = listItem->value;
  fputs(goodPosition->name, file);
  fputs(",", file);
  char amount[256];
  sprintf(amount, "%d", goodPosition->amount);
  fputs(amount, file);
  fputs("\n", file);
}

void writeGoodsPositionListItems(FILE* file, struct ListItem* firstListItem) {
  struct ListItem* listItem = firstListItem;
  while (listItem) {
    writeGoodsPositionListItem(file, listItem);
    listItem = listItem->nextListItem;
  }
}

void writeGoods(struct ListItem* firstListItem) {
  FILE* file = fopen(GOODS_FILE_PATH, "w");
  writeGoodsPositionListItems(file, firstListItem);
  fclose(file);
}

int main(int argc, char* argv[]) {
  char* operation = argv[1];
  char* goodsName = argv[2];
  int amount = atoi(argv[3]);
  if (strcmp(operation, "in") == 0) {
    struct ListItem* firstListItem = readGoods();
    struct ListItem* listItem = findGoodsPositionListItemByName(firstListItem, goodsName);
    if (!listItem) {
      listItem = createAndAppendGoodsPositionListItem(firstListItem, goodsName, 0);
    }
    struct GoodsPosition* goodsPosition = listItem->value;
    goodsPosition->amount += amount;

    writeGoods(firstListItem);
  } else if (strcmp(operation, "out") == 0) {
    struct ListItem* firstListItem = readGoods();
    struct ListItem* listItem = findGoodsPositionListItemByName(firstListItem, goodsName);
    if (listItem) {
      struct GoodsPosition* goodsPosition = listItem->value;
      if (goodsPosition->amount >= amount) {
        goodsPosition->amount -= amount;
        writeGoods(firstListItem);
      } else {
        printf("Amount of good not available. Only %d available.\n", goodsPosition->amount);
      }
    } else {
      puts("Good not available.");
    }
  } else if (strcmp(operation, "set") == 0) {
    struct ListItem* firstListItem = readGoods();
    struct ListItem* listItem = findGoodsPositionListItemByName(firstListItem, goodsName);
    if (!listItem) {
      listItem = createAndAppendGoodsPositionListItem(firstListItem, goodsName, 0);
    }
    struct GoodsPosition* goodsPosition = listItem->value;
    goodsPosition->amount = amount;
    writeGoods(firstListItem);
  }
  return 0;
}

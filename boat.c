#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define some constants
#define MAX_BOATS 120
#define MAX_NAME_LENGTH 127
#define MAX_LINE_LENGTH 100
#define CSV_COM ","

// Define an enumeration for the different types of places where a boat can be stored
enum PlaceType {
    slip,    // In a slip
    land,    // On land
    trailer, // On a trailer
    storage  // In storage
};

// Define a union to hold extra information about each type of place
union ExtraInfoPlaceType {
    int slipNumber;      // Slip number
    char bayLetter;      // Bay letter
    char licenseTag[8];  // Trailer license tag
    int storageSpace;    // Storage space number
};

// Define a struct to hold information about each boat
struct Boat {
    char name[MAX_NAME_LENGTH];             // Boat name
    int length;                             // Boat length
    enum PlaceType type;                    // Type of place where the boat is stored
    union ExtraInfoPlaceType info;          // Extra information about the place where the boat is stored
    float amountOwed;                       // Amount owed for storing the boat
};

// Define a type alias for a pointer to a Boat
typedef struct Boat* BoatPointer;

// Define an array of pointers to Boat, which will hold all of the boats in the marina
typedef BoatPointer AllBoats[MAX_BOATS];

// Define a variable to keep track of the number of boats in the marina
int numOfBoats = 0;

// Define an array to hold pointers to all of the boats in the marina
BoatPointer boats[MAX_BOATS];

// Function to find a boat by name
int findBoat(char *name) {
    for (int i = 0; i < numOfBoats; i++) {
        if (strcasecmp(boats[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to accept a payment for a boat
void acceptPayment(char *name, float amount) {
    int index = findBoat(name);
    if (index == -1) {
        printf("No boat with that name.\n");
        return;
    }
    BoatPointer boat = boats[index];
    if (amount > boat->amountOwed) {
        printf("That is more than the amount owed, $%.2f\n", boat->amountOwed);
        return;
    }

    boat->amountOwed -= amount;
    printf("Payment accepted. New amount owed: $%.2f\n", boat->amountOwed);
}

// Function to compare two boats by name, used for sorting
int compareBoats(const void *a, const void *b) {
    return strcasecmp((*(BoatPointer *) a)->name, (*(BoatPointer *) b)->name);
}

// Function to sort the boat list by name
void sortBoats() {
    qsort(boats, numOfBoats, sizeof(BoatPointer), compareBoats);
}

// Function to update the amount owed for each boat
void updateAmounts() {
    for (int i = 0; i < numOfBoats; i++) {
        switch (boats[i]->type) {
            case slip:
                boats[i]->amountOwed += boats[i]->length * 12.5;
                break;
            case land:
                boats[i]->amountOwed += boats[i]->length * 14.0;
                break;
            case trailer:
                boats[i]->amountOwed += boats[i]->length * 25.0;
                break;
            case storage:
                boats[i]->amountOwed += boats[i]->length * 11.2;
                break;
        }
    }
    printf("Amounts updated for the new month.\n");
}

void payment() {
    char name[MAX_NAME_LENGTH];
    float amount;
    printf("Enter the name of the boat: ");
    scanf(" %[^\n]", name);
    printf("Enter the payment amount: ");
    scanf(" %f", &amount);
    acceptPayment(name, amount);
}
void printBoat(BoatPointer boat){
  switch(boat->type){
    case slip:
        printf("%-20s %3d'    slip   # %2d   Owes $%7.2f\n", boat->name, boat->length, boat->info.slipNumber, boat->amountOwed);
        break;
    case land:
        printf("%-20s %3d'    land      %c   Owes $%7.2f\n", boat->name, boat->length, boat->info.bayLetter, boat->amountOwed);
        break;
    case trailer:
        printf("%-20s %3d' trailer %s   Owes $%7.2f\n", boat->name, boat->length, boat->info.licenseTag, boat->amountOwed);
        break;
    case storage:
        printf("%-20s %3d' storage   # %2d   Owes $%7.2f\n", boat->name, boat->length, boat->info.storageSpace, boat->amountOwed);
        break;
  }
}
void printInventory() { //prints the list of boats using print boats
    printf("\nCurrent Inventory:\n");
    for (int i = 0; i < numOfBoats; i++) {
        printBoat(boats[i]);
    }
}
void saveBoat(const char *filename) { //savles relevant extra information or trows an error
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
    printf("Error opening file for writing.\n");
        return;
        }
        for (int i = 0; i < numOfBoats; i++) {
            BoatPointer boat = boats[i];
            fprintf(file, "%s,%d,", boat->name, boat->length);
            switch (boat->type) {
                case slip:
                    fprintf(file, "slip,%d", boat->info.slipNumber);
                    break;
                case land:
                    fprintf(file, "land,%c", boat->info.bayLetter);
                    break;
                case trailer:
                    fprintf(file, "trailer,%s", boat->info.licenseTag);
                    break;
                case storage:
                    fprintf(file, "storage,%d", boat->info.storageSpace);
                    break;
            }
            fprintf(file, ",%.2f\n", boat->amountOwed);
        }

        fclose(file);
    }
void addBoat(char *line) {
    
    if (numOfBoats == MAX_BOATS) {
        printf("Sorry, the marina is full.\n");
        return;
    }

    BoatPointer boat = (BoatPointer) malloc(sizeof(struct Boat));
    if (boat == NULL) {
        printf("Memory allocation error.\n");
        return;
    }

    char *token;
    token = strtok(line, CSV_COM);
    if (token == NULL || strlen(token) > MAX_NAME_LENGTH) {
        printf("Invalid boat name.\n");
        free(boat);
        return;
    }
    strncpy(boat->name, token, MAX_NAME_LENGTH);
    boat->name[MAX_NAME_LENGTH-1] = '\0'; 
    token = strtok(NULL, CSV_COM);
    boat->length = atoi(token);
    token = strtok(NULL, CSV_COM);
    if (strcasecmp(token, "slip") == 0) {
        boat->type = slip;
        token = strtok(NULL, CSV_COM);
        boat->info.slipNumber = atoi(token);
    }
    else if (strcasecmp(token, "land") == 0) {
        boat->type = land;
        token = strtok(NULL, CSV_COM);
        boat->info.bayLetter = toupper(token[0]);
    }
    else if (strcasecmp(token, "trailer") == 0 || strcasecmp(token, "trailor") == 0) {
        boat->type = trailer;
        token = strtok(NULL, CSV_COM);
        strncpy(boat->info.licenseTag, token, 8);
        boat->info.licenseTag[7] = '\0'; 
    }
    else if (strcasecmp(token, "storage") == 0) {
      boat->type = storage;
      token = strtok(NULL, CSV_COM);
      boat->info.storageSpace = atoi(token);
    }
    else {
        printf("Invalid boat type: %s\n", token);
        free(boat);
        return;
    }
    token = strtok(NULL, CSV_COM);
    if (token != NULL) {
        boat->amountOwed = atoi(token);
    } else {
        printf("Invalid input: missing amount owed\n");
        free(boat);
        return;
    }
    boats[numOfBoats++] = boat;

    sortBoats();
    printf("Boat added.\n");
}

int read_csv(const char* filename) { // read csv line by like fgets till maxboats or end of line
    FILE* fp;
    char line[MAX_LINE_LENGTH];
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Failed to open the file\n");
        return EXIT_FAILURE;
    }
    while (fgets(line, sizeof(line), fp) != NULL & numOfBoats < MAX_BOATS) {
        line[strcspn(line, "\n")] = '\0';
        addBoat(line);
    }
    fclose(fp);

    return EXIT_SUCCESS;
}
void add() { //adds a boat
  char line[256];
  printf("Please enter the boat data in CSV format(boat,length,type,extra): ");
  scanf(" %[^\n]", line);
  addBoat(line);
  }
void removeBoat(char *name) { //remove a boat from numboats and from the array of pointer of boats
    int index = findBoat(name);
    if (index == -1) {
        printf("No boat with that name.\n");
        return;
    }
    BoatPointer boat = boats[index];
    free(boat);

    for (int i = index; i < numOfBoats - 1; i++) {
        boats[i] = boats[i + 1];
    }
    numOfBoats--;

    printf("Boat removed.\n");
}
void removes() { //remove the name & data
    char name[MAX_NAME_LENGTH];
    printf("Enter the name of the boat to remove: ");
    scanf(" %[^\n]", name);
    removeBoat(name);
}



int main(int argc, char *argv[]) {
     if (argc != 2) {
              printf("Usage: %s <boat_data_filename>\n", argv[0]);
              return EXIT_FAILURE;
          }

          const char *boat_data_filename = argv[1];
          read_csv(boat_data_filename);
    printf("Welcome to the Boat Management System\n");
printf("------------------------------------\n");

char option;
do {
    printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : "); //menu
    scanf(" %c", &option);
    option = tolower(option);
    switch (option) {
        case 'i':
            printInventory();
            break;
        case 'a':
            add();
            break;
        case 'r':
            removes();
            break;
        case 'p':
            payment();
            break;
        case 'm':
            updateAmounts();
            break;
        case 'x':
            break;
        default:
            printf("Invalid option %c\n", option);
            break;
    }

} while (option != 'x');
  saveBoat(boat_data_filename);
    return EXIT_SUCCESS;
}

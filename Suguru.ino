 #include <Arduboy2.h>
 
Arduboy2 arduboy;
 
uint8_t height = 7;
uint8_t width = 9;
uint8_t numberOfBlocks = 15;
uint8_t randomCells[5];
uint8_t invalidCells[5];
uint8_t blocksTemplate[7][9];
uint8_t board[7][9];

uint8_t highestAttempt = 0;
uint8_t cellCount = 0;
uint16_t totalAttempts = 0;
bool finished = false;

enum class BoardType : uint8_t {
    BlocksTemplate,
    Board
};


void printBoard() {

    Serial.println("- aa --------------------------------------------------");

    for (uint8_t y = 0; y < height; y++) {

        Serial.print(y);
        Serial.print(" | ");
   
        for (uint8_t x = 0; x < width; x++) {

            if (blocksTemplate[y][x] < 10) Serial.print("0");
            Serial.print(blocksTemplate[y][x]);
            Serial.print(" ");

        }

        Serial.print(" | ");

        for (uint8_t x = 0; x < width; x++) {

            Serial.print(board[y][x]);
            Serial.print(" ");

        }

        Serial.println("");

    }

    Serial.println("");

}

void clearGrid() {

    for (uint8_t y = 0; y < height; y++) {

        for (uint8_t x = 0; x < width; x++) {

            board[y][x] = 0;

        }

    }

}

void clearGrid(uint8_t value) {

    for (uint8_t y = 0; y < height; y++) {

        for (uint8_t x = 0; x < width; x++) {

            if (blocksTemplate[y][x] == value) board[y][x] = 0;

        }

    }

}

uint8_t getCell(BoardType boardType, int8_t x, int8_t y) {

    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }

    switch (boardType) {

        case BoardType::BlocksTemplate:
            return blocksTemplate[y][x];

        case BoardType::Board:
            return board[y][x];

    }

}

// Returns the number of cells for the given block
uint8_t getBlockCellCount(uint8_t block) {
   
    uint8_t i = 0;
   
    for (uint8_t y = 0; y < height; y++) {
   
        for (uint8_t x = 0; x < width; x++) {

            if (getCell(BoardType::BlocksTemplate, x, y) == block) {

                i++;

            }

        }
 
    }
   
    return i;
 
}


void getInvalidCellValues(uint8_t block) {
 
    uint8_t i = 0;
  
    for (uint8_t x = 0; x < 5; x++) {
 
        invalidCells[x] = 0;
     
    }
 
    for (uint8_t y = 0; y < height; y++) {
 
        for (uint8_t x = 0; x < width; x++) {
       
            uint8_t value = getCell(BoardType::BlocksTemplate, x, y);
           
            if (value == block) {
               
                if (getCell(BoardType::Board, x - 1, y - 1) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x - 1, y - 1) - 1));
                }
               
                if (getCell(BoardType::Board, x, y - 1) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x, y - 1) - 1));
                }
               
                if (getCell(BoardType::Board, x + 1, y - 1) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x + 1, y - 1) - 1));
                }
               
                if (getCell(BoardType::Board, x - 1, y) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x - 1, y) - 1));
                }
               
                // if (getCell(BoardType::Board, x, y) != 0) {
                //     invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x, y) - 1));
                // }
               
                if (getCell(BoardType::Board, x + 1, y) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x + 1, y) - 1));
                }
 
                if (getCell(BoardType::Board, x - 1, y + 1) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x - 1, y + 1) - 1));
                }
               
                if (getCell(BoardType::Board, x, y + 1) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x, y + 1) - 1));
                }
               
                if (getCell(BoardType::Board, x + 1, y + 1) != 0) {
                    invalidCells[i] = invalidCells[i] | (1 << (getCell(BoardType::Board, x + 1, y + 1) - 1));
                }
               
                i++;
               
            }
       
        }
 
    }
   
}
 

void addToInvalidCells(uint8_t idx, uint8_t value) {
 
    invalidCells[idx] = invalidCells[idx] | (1 << (value - 1));
 
}

 
// Mix up the placement array.  Only the n first cells are mixed - where n is the number of cells in the current block
bool mixRandomArray(uint8_t cellCount) {
 
    bool singleFound = true;
    uint8_t vacantCells[5];
    uint8_t vacantCellsIdx = 0;
    
    for (uint8_t z = 0; z < 5; z++) {
        randomCells[z] = 0;
    }
   
    
    // Check to see if there are singles ..
   
    singleFound = true;
   
    while (singleFound) {
   
        singleFound = false;

        for (uint8_t z = 0; z < cellCount; z++) {

            switch (cellCount) {
            
                case 1:

                    randomCells[0] = 1;
                    return true;
                
                case 2:
            
                    for (uint8_t x = 0; x < cellCount; x++) {

                        if (randomCells[x] == 0) {
                    
                            switch (invalidCells[x]) {

                                case 2:

                                    randomCells[x] = 1;
                                    addToInvalidCells(x, 1);
                                    singleFound = true;
                                    break;
                        
                                case 1:

                                    randomCells[x] = 2;
                                    addToInvalidCells(x, 2);
                                    singleFound = true;
                                    break;
                        
                                case 3:
                                    return false;
                                
                            }
                        
                        }
                    
                    }
                
                    break;
                
                case 3:
            
                    for (uint8_t x = 0; x < cellCount; x++) {
                
                        if (randomCells[x] == 0) {
                        
                            switch (invalidCells[x]) {
                        
                                case 6:
                                    randomCells[x] = 1;
                                    addToInvalidCells(x, 1);
                                    singleFound = true;
                                    break;                           
                                    
                                case 5:
                                    randomCells[x] = 2;
                                    addToInvalidCells(x, 2);
                                    singleFound = true;
                                    break;                           
                            
                                case 3:
                                    randomCells[x] = 3;
                                    addToInvalidCells(x, 3);
                                    singleFound = true;
                                    break;                                 
                            
                                case 7:
                                    return false;
                                
                            }
                        
                        }
                        
                    }

                    break;
                
                case 4:
            
                    for (uint8_t x = 0; x < cellCount; x++) {
                
                        if (randomCells[x] == 0) {
                    
                            switch (invalidCells[x]) {
                        
                                case 14:
                                    randomCells[x] = 1;
                                    addToInvalidCells(x, 1);
                                    singleFound = true;
                                    break;                
                                                                
                                case 13:
                                    randomCells[x] = 2;
                                    addToInvalidCells(x, 2);
                                    singleFound = true;
                                    break;                
                            
                                case 11:
                                    randomCells[x] = 3;
                                    addToInvalidCells(x, 3);
                                    singleFound = true;
                                    break;                
                            
                                case 7:
                                    randomCells[x] = 4;
                                    addToInvalidCells(x, 4);
                                    singleFound = true;
                                    break;                
                            
                                case 15:
                                    return false;
                                
                            }
                        
                        }
                        
                    }

                    break;
            
                case 5:
            
                    for (uint8_t x = 0; x < cellCount; x++) {
                
                        if (randomCells[x] == 0) {
                    
                            switch (invalidCells[x]) {
                        
                                case 30:
                                    randomCells[x] = 1;
                                    addToInvalidCells(x, 1);
                                    singleFound = true;
                                    break;           
                            
                                case 29:
                                    randomCells[x] = 2;
                                    addToInvalidCells(x, 2);
                                    singleFound = true;
                                    break;           
                            
                                case 27:
                                    randomCells[x] = 3;
                                    addToInvalidCells(x, 3);
                                    singleFound = true;
                                    break;           
                            
                                case 23:
                                    randomCells[x] = 4;
                                    addToInvalidCells(x, 4);
                                    singleFound = true;
                                    break;           
                            
                                case 15:
                                    randomCells[x] = 5;
                                    addToInvalidCells(x, 5);
                                    singleFound = true;
                                    break;      
                            
                                case 31:
                                    return false;
                                
                            }
                        
                        }
                    
                    }

                    break;
                
            }
    
        }
       
    }
  
      
    // Place remainders randomly ..
   
    bool found = false;

    for (uint8_t cell = 1; cell <= cellCount; cell++) {
   
        found = false;
       
        // is it already placed?
       
        for (uint8_t x = 0; x < cellCount; x++) {
       
            if (randomCells[x] == cell) {
            
                found = true;
                break;
               
            }
           
        }
   
            
        while (!found) {
           
            vacantCellsIdx = 0;
           
            for (uint8_t x = 0; x < cellCount; x++) {
           
                vacantCells[x] = 0;
               
                if (randomCells[x] == 0) {
               
                    vacantCells[vacantCellsIdx] = x;
                    vacantCellsIdx = vacantCellsIdx + 1;
                   
                }
               
            }

// printf("vacantCells pre mix :");
// for (uint8_t x = 0; x < cellCount; x++) {
// printf("%i ", vacantCells[x]);
// }
// printf("\n");
        
            // Now mix the cells up ..
           
            if (vacantCellsIdx > 1) {
 
                for (uint8_t attempts = 1; attempts < vacantCellsIdx * 4; attempts++) {
               
                    uint8_t x = random(0, vacantCellsIdx);
                    uint8_t y = random(0, vacantCellsIdx);
                   
                    uint8_t swap = vacantCells[x];
                    vacantCells[x] = vacantCells[y];
                    vacantCells[y] = swap;
                   
                }
               
            }

// printf("CEll to place %i, vacantCells post mix :", cell);
// for (uint8_t x = 0; x < cellCount; x++) {
// printf("%i ", vacantCells[x]);
// }
// printf("- vacantCellsIdx %i : ", vacantCellsIdx);

// printf("randomCells before :");
// for (uint8_t x = 0; x < cellCount; x++) {
// printf("%i ", randomCells[x]);
// }
// printf(", ");
        
            for (uint8_t attempts = 0; attempts < vacantCellsIdx; attempts++) {
               
                uint8_t randomCell = vacantCells[attempts];
           
                if (randomCells[randomCell] == 0) {
               
                    if ((invalidCells[randomCell] & (1 << (cell - 1))) == 0) {
                   
                        randomCells[randomCell] = cell;
                        found = true;
                        break;
                   
                    }
                   
                }
               
            }
// printf("randomCells after :");
// for (uint8_t x = 0; x < cellCount; x++) {
// printf("%i ", randomCells[x]);
// }
// printf("\n");          
            if (!found) {
           
                return false;
               
            }
       
        }
       
    }
   
    return true;
   
}
 
// Set the indexed cell in the nominated block with a value
void setCell(uint8_t block, uint8_t index, uint8_t value) {

    uint8_t i = 0;

    for (uint8_t y = 0; y < height; y++) {
   
        for (uint8_t x = 0; x < width; x++) {
       
            if (getCell(BoardType::BlocksTemplate, x, y) == block) {
               
                if (i == index) {

                    board[y][x] = value;
                    return;
                       
                }
           
                i = i + 1;
               
            }
       
        }
       
    }
   
}

void build() {

    blocksTemplate[0][0] = 15;
    blocksTemplate[1][0] = 15;
    blocksTemplate[2][0] = 15;
    blocksTemplate[3][0] = 14;
    blocksTemplate[4][0] = 14;
    blocksTemplate[5][0] = 13;
    blocksTemplate[6][0] = 13;

    blocksTemplate[0][1] = 15;
    blocksTemplate[1][1] = 15;
    blocksTemplate[2][1] = 14;
    blocksTemplate[3][1] = 14;
    blocksTemplate[4][1] = 12;
    blocksTemplate[5][1] = 13;
    blocksTemplate[6][1] = 13;

    blocksTemplate[0][2] = 10;
    blocksTemplate[1][2] = 10;
    blocksTemplate[2][2] = 11;
    blocksTemplate[3][2] = 14;
    blocksTemplate[4][2] = 12;
    blocksTemplate[5][2] = 12;
    blocksTemplate[6][2] = 12;

    blocksTemplate[0][3] = 10;
    blocksTemplate[1][3] = 10;
    blocksTemplate[2][3] = 11;
    blocksTemplate[3][3] = 11;
    blocksTemplate[4][3] = 11;
    blocksTemplate[5][3] = 12;
    blocksTemplate[6][3] = 8;

    blocksTemplate[0][4] = 10;
    blocksTemplate[1][4] = 9;
    blocksTemplate[2][4] = 9;
    blocksTemplate[3][4] = 11;
    blocksTemplate[4][4] = 8;
    blocksTemplate[5][4] = 8;
    blocksTemplate[6][4] = 8;

    blocksTemplate[0][5] = 9;
    blocksTemplate[1][5] = 9;
    blocksTemplate[2][5] = 6;
    blocksTemplate[3][5] = 7;
    blocksTemplate[4][5] = 7;
    blocksTemplate[5][5] = 8;
    blocksTemplate[6][5] = 4;

    blocksTemplate[0][6] = 9;
    blocksTemplate[1][6] = 6;
    blocksTemplate[2][6] = 6;
    blocksTemplate[3][6] = 7;
    blocksTemplate[4][6] = 7;
    blocksTemplate[5][6] = 7;
    blocksTemplate[6][6] = 4;

    blocksTemplate[0][7] = 6;
    blocksTemplate[1][7] = 6;
    blocksTemplate[2][7] = 1;
    blocksTemplate[3][7] = 5;
    blocksTemplate[4][7] = 2;
    blocksTemplate[5][7] = 3;
    blocksTemplate[6][7] = 3;

    blocksTemplate[0][8] = 5;
    blocksTemplate[1][8] = 5;
    blocksTemplate[2][8] = 5;
    blocksTemplate[3][8] = 5;
    blocksTemplate[4][8] = 3;
    blocksTemplate[5][8] = 3;
    blocksTemplate[6][8] = 3;


    uint8_t attempts = 0;

       
    // do {

        totalAttempts++;
       

        // Clear grid before starting ..

        clearGrid();
        finished = true;
           
        // Iterate through all block
        for (uint8_t i = 1; i <= numberOfBlocks; i++) {
       
            // printf("Block : %i\n", i);

            attempts = 0;


            // Generate a random placement
            
            cellCount = getBlockCellCount(i);
            getInvalidCellValues(i);

            do {
                   
                // attempt to mix cells multiple times ...

                bool randomizeOK = mixRandomArray(cellCount);       

                if (randomizeOK) {
                   
                    for (uint8_t j = 0; j < cellCount; j++) {
                        setCell(i, j, randomCells[j]);
                    }

                    break;
                   
                }
 
               
                // If we have done this 20 times then abort .. its probably invalid.
                attempts = attempts + 1;
                if (attempts == 20) break;
            
            } while (true);
           
            
            // Exit the block iteration and start again if its unsolvable.
            if (attempts == 20) {
           
                if (i >= highestAttempt) {
                    highestAttempt = i;
                    Serial.print("Highest : ");
                    Serial.println(i);
                    printBoard();
                    
                }

//                printf("unable to complete %i, winding back to %i\n", i , i - 2);
                finished = false;   
                break;

            }

            // printBoard();
       
        }

Serial.print("totalAttempts : ");
Serial.println(totalAttempts);           

    // } while (!finished);


    //printf("complete!\n");
    printBoard();

}
 
  
 

void setup() {
 
    arduboy.begin();
    arduboy.setFrameRate(15);

}

void loop() {

    //Prevent the Arduboy from running too fast
    if (!arduboy.nextFrame()) {
        return;
    }

    arduboy.clear();
    arduboy.pollButtons();

    if (!finished) build();

    arduboy.display();

}
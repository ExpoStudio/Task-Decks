#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define ACCESS _access
#else
#include <unistd.h>
#define ACCESS access
#endif

#define foreach(element_var, array) \
    for (size_t _i = 0; _i < sizeof(array)/sizeof(array[0]); _i++) \
        for (element_var = &(array[_i]); element_var != NULL; element_var = NULL)


//STRUCT FOR ALL ELEMENTS OF A SINGLE TASK
typedef struct
{
    char TaskName[80];
    char Task_Description[400];
    //Enter a number of at least 0, Clamp this between 0 and 100
    int priority_range;
    //Simply the order the task is in the set. Show the IDs in the editor only
    int id;
} Tasks;

//ARRAY OF ALL TASKS IN ONE SAVE
typedef struct
{
    char TaskDeckName[50];
    Tasks tasks[30];
    //Order in the Array
    int id;
    bool newlyCreated;
} TaskDeck;

//BINARY SAVE OF ALL SAVE FILES
typedef struct
{
    TaskDeck userSavedTaskDecks[10];
} AllSaves;

enum SystemState
{
    TitleScreen,
    CreateNewFile,
    LoadFile,
    CreateNewTaskDeck,
    CreateNewTask,
    EditTaskDeck,
    TaskTracker,
    EditTask,
    TaskDeckSettings
};
char a,b,c,taskOperationChoice[3];
//Prototypes
bool fileFound;
enum SystemState AppState = TitleScreen;
void SaveToFile(const char* filename, const AllSaves* state);
int ReadSaveFile(const char* filename, AllSaves* state);
void InitializeState(AllSaves* state);
void ClearScreen();
void DeleteTask(Tasks* task);
int FindNextEmptyTask(TaskDeck* deck, size_t deckLength);
void Clamp(int* variable, int min, int max);
Tasks RecentlyCompletedTask;
Tasks SelectedTask;
int SelectedTaskIndex;

int main()
{
    AllSaves loadedSave;
    int options_choice;
    int saveFileChosen = -1;
    const char* fileName = "SavedTaskSets";

    strcpy(RecentlyCompletedTask.TaskName, "null");
    strcpy(RecentlyCompletedTask.Task_Description, "null");
    RecentlyCompletedTask.priority_range = -1;
    RecentlyCompletedTask.id = -1;
    int choice = -1;
    size_t count = 0;
    while(true)
    {
        switch(AppState)
        {
            case TitleScreen:
                {
                    printf("Hello, welcome to task editor! Select an option to begin!\n");
                    printf("\t (1) Create New Task Set");
                    if (ReadSaveFile(fileName, &loadedSave))
                    {
                        printf("\t (2) Load file\n");
                        fileFound = true;
                    }
                    else
                    {
                        printf("\t\033[90m(*) -- No Save File Found\033[0m\n");
                        fileFound = false;
                    }



                    for(int loop = 1; loop > 0; loop++)
                    {
                        scanf("%d", &options_choice);
                        while ((getchar()) != '\n');

                        if (options_choice == 1)
                        {
                            AppState = CreateNewFile;
                            printf("Press enter to continue...\n");
                            getchar();
                            ClearScreen();
                            break;

                        }
                        else if (options_choice == 2 && fileFound)
                        {
                            AppState = LoadFile;
                            printf("Press enter to continue...\n");
                            getchar();
                            ClearScreen();
                            break;
                        }
                        else
                        {
                            ClearScreen();
                            printf("That Option is not valid, Try Again\n");
                            printf("Hello, welcome to task editor! Select an option to begin!\n");
                            printf("\t (1) Create New Task Set");
                            printf("\t (2) Load file\n");
                        }
                    }
                }
                break;
            case LoadFile:
                ClearScreen();
                printf("Awesome! Type a number to open up a Task Deck. To make a new task deck, select a number that corresponds to an empty slot.\n");
                while (true)
                {

                    for (int loopKeepingTrack = 0; loopKeepingTrack < 10; loopKeepingTrack++)
                    {
                        if (loadedSave.userSavedTaskDecks[loopKeepingTrack].newlyCreated)
                        {
                            printf("\t (%d) -- Empty Slot --\n", loopKeepingTrack+1);
                        }
                        else
                        {
                            printf("\t (%d) -- Load Task Deck '%s'\n", loopKeepingTrack+1, loadedSave.userSavedTaskDecks[loopKeepingTrack].TaskDeckName);
                        }
                    };


                    while (true)
                    {
                        int options_choice;
                        scanf("%d", &options_choice);
                        while ((getchar()) != '\n');

                        if (options_choice < 1 || options_choice > 10)
                        {
                            printf("That option is not valid, try again");
                            continue;
                        }

                        options_choice -= 1;
                        saveFileChosen = options_choice;
                        if (loadedSave.userSavedTaskDecks[options_choice].newlyCreated)
                        {
                            AppState = CreateNewTaskDeck;
                            loadedSave.userSavedTaskDecks[saveFileChosen].id = options_choice;
                            printf("Empty save slot %d chosen, press enter to continue...\n", options_choice+1);
                            getchar();
                            ClearScreen();
                            break;
                        }

                        printf("Save slot %d chosen, press enter to continue...\n", options_choice+1);
                        getchar();
                        AppState = TaskTracker;
                        ClearScreen();
                        break;
                    };
                    break;
                }
                break;
            case CreateNewFile:
                printf("Are you sure? This will completely erase all saved task sets\n");
                printf("\t (1) Yes\t (0) No\n");
                choice = -1;
                scanf("%d", &choice);
                if (choice == 1)
                {
                    InitializeState(&loadedSave);
                    SaveToFile(fileName, &loadedSave);
                    printf("Press Enter To continue\n");
                    getchar();
                    AppState = LoadFile;
                }
                else if (choice == 0)
                {
                    printf("Aborting. Press enter to continue\n");
                    getchar();
                    AppState = TitleScreen;
                    ClearScreen();
                }
                else
                {
                    printf("Invalid Option, aborting. Press enter to continue\n");
                    getchar();
                    AppState = TitleScreen;
                    ClearScreen();
                }
                break;
            case CreateNewTaskDeck:
                printf("Give your task deck a good name (or not): ");
                scanf("%49[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].TaskDeckName);
                while ((getchar()) != '\n');
                printf("\nNice, your deck will be named %s\n", loadedSave.userSavedTaskDecks[saveFileChosen].TaskDeckName);
                loadedSave.userSavedTaskDecks[saveFileChosen].newlyCreated = false;
                SaveToFile(fileName, &loadedSave);
                getchar();
                printf("Lets make your first task!\n");

                printf("Enter a task name: \n");
                scanf("%79[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0].TaskName);
                while ((getchar()) != '\n');

                printf("Enter a task description: \n");
                scanf("%399[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0].Task_Description);
                while ((getchar()) != '\n');

                printf("Enter a priority range (from 0 - 100)\n");
                scanf("%99d", &loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0].priority_range);
                Clamp(&loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0].priority_range, 0, 100);
                while ((getchar()) != '\n');

                printf("Awesome! Enter to continue\n");
                getchar();
                AppState = TaskTracker;
                loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0].id = 0;
                SaveToFile(fileName, &loadedSave);
                break;
            case CreateNewTask:
                printf("Enter a task name: \n");
                int index = FindNextEmptyTask(&loadedSave.userSavedTaskDecks[saveFileChosen], 30);

                scanf("%79[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[index].TaskName);
                while ((getchar()) != '\n');

                printf("Enter a task description: \n");
                scanf("%399[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[index].Task_Description);
                while ((getchar()) != '\n');

                printf("Enter a priority range (from 0 - 100)\n");
                scanf("%d", &loadedSave.userSavedTaskDecks[saveFileChosen].tasks[index].priority_range);
                Clamp(&loadedSave.userSavedTaskDecks[saveFileChosen].tasks[index].priority_range, 0, 100);
                while ((getchar()) != '\n');

                printf("Awesome! Enter to continue\n");
                getchar();
                loadedSave.userSavedTaskDecks[saveFileChosen].tasks[index].id = index;
                AppState = TaskTracker;
                SaveToFile(fileName, &loadedSave);
                break;
            case TaskTracker:
                ClearScreen();
                printf("Here are your available tasks; get to it!\n");
                printf("====================================\n");
                printf("Task Deck '%s'\n", loadedSave.userSavedTaskDecks[saveFileChosen].TaskDeckName);
                size_t deckLength = sizeof(loadedSave.userSavedTaskDecks[saveFileChosen].tasks)/sizeof(loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0]);
                if (strcmp(RecentlyCompletedTask.TaskName, "null") != 0)
                {
                    printf("\033[38;5;28mRecently Completed - '%s'\033[0m\n",RecentlyCompletedTask.TaskName);
                    printf("\033[38;5;28m\t -- Task Description: %s \033[0m\n",RecentlyCompletedTask.Task_Description);
                }
                for(int i = 0; i < (deckLength); i++)
                {
                    if (strcmp(loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].TaskName, "null") == 0)
                    {
                         continue;
                    }

                    printf("(*) - '%s'\n",loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].TaskName);
                    printf("\t -- Desc: %s\n", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].Task_Description);
                    printf("\t -- ID: %d\n", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].id);

                    int priority = loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range;
                    switch (priority / 10)
                    {
                        case 10:
                            printf("\033[1;41m\t - (*****) Priority Level (MAXIMAL - TOP PRIORITY) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 9:
                            printf("\033[1;31m\t - (****) Priority Level (URGENT!!) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 8:
                            printf("\033[0;31m\t - (***) Priority Level (Very Important) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 7:
                            printf("\033[38;5;208m\t - (**) Priority Level (Important) %d\033[0m\n",  // orange
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 6:
                        case 5:
                            printf("\033[1;33m\t - (**) Priority Level (Medium) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 4:
                        case 3:
                            printf("\033[0;33m\t - (*) Priority Level (Low) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 2:
                        case 1:
                            printf("\033[0;37m\t - (--) Priority Level (Minimal) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                        case 0:
                        default:
                            printf("\033[90m\t - (-) Priority Level (Minimal) %d\033[0m\n",
                                   loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].priority_range);
                            break;
                    }
                }
                while(true)
                {
                    printf("====================================\n");
                    printf("Select an option\n");
                    printf("\t ( ) - Enter Task ID number to select a task\n");
                    printf("\t (*) - Enter '*' to create new task\n");
                    printf("\t (-) - Enter '-' to open task deck settings\n");

                    scanf("%3s", taskOperationChoice);
                    while ((getchar()) != '\n');
                    bool hasNumber = true;

                    for(int i = 0; taskOperationChoice[i] != '\0'; i++)
                    {
                        if (!isdigit((unsigned char)taskOperationChoice[i]))
                        {
                            hasNumber = false;
                            break;
                        }
                    }

                    if (hasNumber)
                    {
                        bool found = false;
                        int SelectedID = atoi(taskOperationChoice);
                        for(int i = 0; i < deckLength; i++)
                        {
                            if (SelectedID == loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i].id)
                            {
                                SelectedTask = loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i];
                                SelectedTaskIndex = i;
                                AppState = EditTask;
                                found = true;
                                DeleteTask(&RecentlyCompletedTask);
                                printf("Chose Task %s\n", SelectedTask.TaskName);
                                break;
                            }
                        }
                        if (!found)
                        {
                            printf("No task found with given ID number, try again\n");
                            DeleteTask(&RecentlyCompletedTask);
                        }
                        else if (found)
                        {
                            break;
                        }
                    }

                    else if (!hasNumber)
                    {
                        a = taskOperationChoice[0];
                        if (a == '*')
                        {
                            printf("Create New Task Chosen, press enter to continue\n");
                            DeleteTask(&RecentlyCompletedTask);
                            getchar();
                            AppState = CreateNewTask;
                            break;
                        }
                        else if(a=='-')
                        {
                            printf("Opening task deck settings, press enter to continue\n");
                            DeleteTask(&RecentlyCompletedTask);
                            getchar();
                            AppState = TaskDeckSettings;
                            break;
                        }
                    }
                    else
                    {
                        printf("Unknown operation or task id, try again\n");
                        DeleteTask(&RecentlyCompletedTask);
                    }
                }
                break;
            case TaskDeckSettings:
                ClearScreen();
                printf("What would you like to do with this task deck?\n");
                printf("- (0) - Rename Task Deck\n");
                printf("\t -- Current Name: %s\n", loadedSave.userSavedTaskDecks[saveFileChosen].TaskDeckName);
                printf("- (1) - Delete Task Deck\n");
                printf("- (2) - Return to Main Menu\n");
                printf("- (3) - Return to Task Deck\n");
                int choice = -1;
                while(true)
                {
                    scanf("%d",&choice);
                    while ((getchar()) != '\n');

                    bool loopOver = true;
                    if (choice == 0)
                    {
                        printf("Enter a new name: ");
                        scanf("%49[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].TaskDeckName);
                        while ((getchar()) != '\n');

                        printf("Successfully renamed, press enter to continue\n");
                        getchar();
                        loopOver = false;
                    }
                    else if (choice == 1)
                    {
                        printf("Are you sure?\n");
                        printf("\t (1) Yes \t (0) No\n");
                        int c = -1;
                        scanf("%d",&c);
                        while ((getchar()) != '\n');
                        switch(c)
                        {
                        case 1:
                            count = sizeof(loadedSave.userSavedTaskDecks[saveFileChosen].tasks)/sizeof(loadedSave.userSavedTaskDecks[saveFileChosen].tasks[0]);
                            for(int i = 0; i < count; i++)
                            {
                                DeleteTask(&loadedSave.userSavedTaskDecks[saveFileChosen].tasks[i]);
                            }
                            loadedSave.userSavedTaskDecks[saveFileChosen].newlyCreated = true;
                            strcpy(loadedSave.userSavedTaskDecks[saveFileChosen].TaskDeckName, "null");
                            SaveToFile(fileName, &loadedSave);
                            printf("Task deck successfully deleted, press enter to continue\n");
                            loopOver = false;
                            getchar();

                            AppState = LoadFile;
                            break;
                        case 0:
                            printf("Aborting, press enter to continue");
                            getchar();

                            loopOver = true;
                            break;
                        default:
                            printf("Invalid option, aborting. Press enter to continue");
                            getchar();

                            loopOver = true;
                            break;
                        }
                    }
                    else if(choice == 2)
                    {
                        saveFileChosen = -1;
                        DeleteTask(&RecentlyCompletedTask);
                        DeleteTask(&SelectedTask);
                        AppState = TitleScreen;
                        loopOver = false;
                    }
                    else if(choice == 3)
                    {
                        ClearScreen();
                        AppState = TaskTracker;
                        loopOver = false;
                    }
                    else
                    {
                        printf("Invalid option, try again");
                    }

                    if (!loopOver) break;
                }
                break;
            case EditTask:
                ClearScreen();
                printf("What would you like to do with task '%s'?\n",SelectedTask.TaskName);
                printf("\t - (1) - Mark task as completed\n");
                printf("\t - (2) - Edit Task Name\n");
                printf("\t - (3) - Edit Task Description\n");
                printf("\t -- Current Description: %s\n",SelectedTask.Task_Description);
                printf("\t - (4) - Edit Task Priority (%d)\n", SelectedTask.priority_range);
                printf("\t - (5) - Delete Task\n");
                printf("\t - (6) - Exit\n");

                int c = -1;
                scanf("%d", &c);
                while ((getchar()) != '\n');
                switch(c)
                {
                    case 1:
                        RecentlyCompletedTask = loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex];
                        DeleteTask(&SelectedTask);
                        DeleteTask(&loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex]);
                        printf("Task Completed!\n");
                        AppState = TaskTracker;
                        SaveToFile(fileName, &loadedSave);
                        break;
                    case 2:
                        printf("Enter a name: ");
                        scanf("%80[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex].TaskName);
                        while ((getchar()) != '\n');
                        SelectedTask = loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex];
                        printf("Successfully renamed to '%s', press enter to continue\n", SelectedTask.TaskName);
                        getchar();
                        SaveToFile(fileName, &loadedSave);
                        break;
                    case 3:
                        printf("Old Description\n");
                        printf("============================\n");
                        printf("%s\n",SelectedTask.Task_Description);
                        printf("============================\n");
                        printf("Type a new description: ");
                        scanf("%400[^\n]s", loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex].Task_Description);
                        while ((getchar()) != '\n');
                        SelectedTask = loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex];
                        printf("Successfully edited, press enter to continue\n");
                        getchar();
                        SaveToFile(fileName, &loadedSave);
                        break;
                    case 4:
                        printf("Enter new priority: \n");
                        scanf("%d", &loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex].priority_range);
                        while ((getchar()) != '\n');
                        SelectedTask = loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex];
                        printf("Successfully edited, press enter to continue\n");
                        getchar();
                        SaveToFile(fileName, &loadedSave);
                        break;
                    case 5:
                        printf("Are you sure you want to delete task '%s'?\n", SelectedTask.TaskName);
                        printf("\t (1) Yes \t (0) No\n");
                        int a = 0;
                        scanf("%d",&a);
                        while ((getchar()) != '\n');
                        if (a==0)
                        {
                            printf("No selected, Aborting. Press enter to continue\n");
                            getchar();
                        }
                        else if (a==1)
                        {
                            DeleteTask(&loadedSave.userSavedTaskDecks[saveFileChosen].tasks[SelectedTaskIndex]);
                            printf("Task Successfully Deleted. Press enter to continue");
                            AppState = TaskTracker;
                            getchar();
                            SaveToFile(fileName, &loadedSave);
                        }
                        else
                        {
                            printf("No valid option chosen, aborting. Press enter to continue");
                            getchar();
                        }
                        break;
                    case 6:
                        AppState = TaskTracker;
                        break;
                    default:
                        printf("Unknown Number, Try Again.");
                        getchar();
                        break;
                }
            default:
            {
                break;
            }
        };
    }
    return 0;
}

///Returns an int representing an index
int FindNextEmptyTask(TaskDeck* deck, size_t deckLength)
{
    for(int i = 0; i < deckLength; i++)
    {
        if (strcmp(deck->tasks[i].TaskName, "null") == 0)
        {
            return i;
        }
        else continue;
    }
    return 0;
}

void Clamp(int* variable, int min, int max)
{
    *variable = *variable > max ? max : *variable;
    *variable = *variable < min ? min : *variable;
}

void InitializeState(AllSaves* state)
{
    size_t decks = sizeof(state->userSavedTaskDecks) / sizeof(state->userSavedTaskDecks[0]);
    size_t tasksPerDeck = sizeof(state->userSavedTaskDecks[0].tasks) / sizeof(state->userSavedTaskDecks[0].tasks[0]);

    for(size_t loopTrack = 0; loopTrack < decks; loopTrack++)
    {
       state->userSavedTaskDecks[loopTrack].newlyCreated = true;
       state->userSavedTaskDecks[loopTrack].id = loopTrack;
       strcpy(state->userSavedTaskDecks[loopTrack].TaskDeckName, "null");
       for(size_t loop = 0; loop < tasksPerDeck; loop++)
       {
           strcpy(state->userSavedTaskDecks[loopTrack].tasks[loop].TaskName, "null");
       };
    };
}

void DeleteTask(Tasks* task)
{
    strcpy(task->TaskName,"null");
    strcpy(task->Task_Description,"null");
    task->priority_range = -1;
    task->id = -1;
}

void SaveToFile(const char* filename, const AllSaves* state)
{
    FILE* file = fopen(filename, "wb");
    if(file == NULL)
    {
        printf("Error: File could not be opened\n");
        return;
    }
    fwrite(state, sizeof(AllSaves),1, file);
    fclose(file);
}

int ReadSaveFile(const char* filename, AllSaves* state)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    fread(state, sizeof(AllSaves), 1, file);
    fclose(file);
    return 1;
}

void ClearScreen()
{
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif // _WIN32
}


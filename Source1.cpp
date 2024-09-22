/* 加載預設的頭文件 */

#include <algorithm> // For std::sort
#include <cctype> // For digit check
#include <chrono> // For timing
#include <cmath> // For C math functions
#include <cstdint> // For specific data type
#include <ctime> // For time related functions
#include <iomanip> // For setw etc
#include <iostream> // For print and user input
#include <limits> // For areyousure()
#include <regex> // For std::regex pattern (limit user to redeem only shown vector gifts) *This will increase the compile time*
#include <sstream> // For dataStream
#include <string> // For std::string data type
// #include <string_view> // For std::string_view (已移除)
#include <thread> // For std::thread
#include <vector> // For std::vector
#include <Windows.h> // Also for timing and clear

/* 此功能爲開發人員選項 請勿開啟 啟用時請移除最前方的兩個斜號 */

// #define DEBUG_ENABLED

/*********************************************************/

/* Thank you my hero Windows.h */
void SetConsoleColor(WORD color) // XD
{
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsoleOutput, color);
}

bool areYouSure(const std::string& clear, const std::string& type, const std::string check, const std::string customerID) // use for [3] edit customer , [4] enter customer view and [5] show transaction history
{
    char select{ 0 };

    std::string selectStr{};

    if (check == "check")
    {
        std::cin.clear();

        std::cin.ignore(10000, '\n');
    }

    do
    {
        if (type == "exit")
        {
            std::cout << "Are you sure you want to ";
            
            SetConsoleColor(12);
            std::cout << "leave";
            SetConsoleColor(7);
            
            std::cout << "? (Y/N): ";
        }
        else if(type == "delete")
        {
            SetConsoleColor(12);
            std::cout << "Are you sure you want to delete " << customerID << " 's Data? (Y/N): ";
            SetConsoleColor(7);
        }
        else
        {
            std::cout << "Are you sure you want to ";

            SetConsoleColor(10);
            std::cout << "continue";
            SetConsoleColor(7);

            std::cout << "? (Y/N): ";
        }

        std::getline(std::cin, selectStr);

        if (clear == "clear")
            system("cls");

        if (selectStr.length() == 1 && (selectStr[0] == 'Y' || selectStr[0] == 'y' || selectStr[0] == 'N' || selectStr[0] == 'n'))
        {
            select = selectStr[0];  // Accept the valid input
        }
        else
        {
            select = 0;  // Reset select to continue the loop
        }

    } while (select != 'Y' && select != 'y' && select != 'N' && select != 'n');

    if (select == 'N' || select == 'n')
    {
        if (clear == "clear")
            system("cls");

        return false;
    }

    if (clear == "clear")
        system("cls");

    return true;
}

char getUserInput(bool dataLoaded)
{
    char userInput{ 0 }; // 初始化 userInput 數值 (賦予 0 可以避免未定義的風險) 謹記此爲 char 並非 int

    std::string inputBuffer{};

    bool DEBUG_MODE = false;

    #ifdef DEBUG_ENABLED
    DEBUG_MODE = true;
    #endif

    do
    {
        SetConsoleColor(14);
        std::cout << "Welcome! Customer" << std::endl; // 歡迎信息 (待更改)
        SetConsoleColor(7);
        std::cout << "*** Main Menu ***" << '\n';
        std::cout << "[1] Load Starting Data" << '\n';
        std::cout << "[2] Show Records" << '\n';
        std::cout << "[3] Edit Customers" << '\n';
        std::cout << "[4] Enter Customer View" << '\n';
        std::cout << "[5] Show Transaction History" << '\n';
        std::cout << "[6] Credits and Exit" << '\n';

        if (DEBUG_MODE)
        {
            std::cout << std::endl;
            std::cout << "*** Creative Mode ***" << '\n';
            std::cout << "[7] Find Initial Data" << '\n';
            std::cout << "[8] Delete Customer Data" << '\n';
            std::cout << "[9] Edit CC Point" << '\n';
            std::cout << "*****************" << '\n';
            std::cout << "Option (0 - 9): ";
        }
        else
        {
            std::cout << "*****************" << '\n';
            std::cout << "Option (1 - 6): ";
        }

        std::cin >> inputBuffer;

        if (inputBuffer.length() == 1)
        {
            userInput = inputBuffer[0];
        }
        else
        {
            userInput = 0;
        }

        system("cls");

        #ifndef DEBUG_ENABLED // Debug 模式時跳過檢查

        if (!dataLoaded && userInput != '1' && userInput != '6') // 規定用戶必須輸入 [1] 或 [6]，否則 [2] - [5] 將無法使用
        {
            system("cls");

            std::cout << "Please Load the Starting Data First" << '\n';

            Sleep(1500);

            system("cls");

            userInput = '0'; // 將 userInput 設為無效值以便繼續進行 loop
        }

        #endif

    } while (userInput < '1' || (userInput > '6' && !(DEBUG_MODE && userInput <= '9')));

    return userInput; // 返回用戶輸入
}

#ifndef DEBUG_ENABLED // Debug 模式時跳過進度條

void progressBar(int progress, int total) // 進度條函數
{
    int barWidth = 50;
    int filledWidth = static_cast<int>(barWidth * progress / total);
    std::cout << '[';
    for (int i{ 0 }; i < barWidth; ++i)
    {
        if (i < filledWidth) std::cout << '=';
        else if (i == filledWidth) std::cout << '>';
        else std::cout << ' ';
    }
    std::cout << ']' << ' ' << std::setw(3);
    if (((progress * 100) / total) == 100)
        SetConsoleColor(10);
    else
        SetConsoleColor(12);
    std::cout << (progress * 100 / total) << "%\r";
    std::cout.flush();
    SetConsoleColor(7);
}

#endif

class GiftRecord // struct, 用來儲存 gift record 的數據 因應功課要求變成 class
{
public:
    std::string giftID{};
    std::string giftDescription{};
    int giftPrice{};
    int giftPointsRequired{};

    // 避免 giftDescription 超過 100 char which is impossible so this is just to satisfy 要求 and wont be use XD

    void setGiftDescription(const std::string& description)
    {
        if (description.length() > 100)
        {
            giftDescription = description.substr(0, 100);
        }
        else
        {
            giftDescription = description;
        }
    }
};

struct InitialCustomerRecord // 儲存第一次的數據 以便用戶在 [5] 時查閱數據
{
    std::string initialCustomerID{};
    int initialPointsBalance{};
};

struct CustomerRecord // struct, 用來儲存 customer record 的數據
{
    std::string customerID{};
    char customerRank{};
    int pointsBalance{};
};

struct TransactionRecord // struct, 用來儲存 transaction record 的數據
{
    std::string customerID{};
    std::string type{};
    std::string details{};
    int pointsChange{};
    double extraMoney{};
};

std::vector<GiftRecord> giftRecords; // 儲存禮品記錄的 vector
std::vector<InitialCustomerRecord> initialCustomerRecords; // 儲存初始禮品記錄數據的 vector
std::vector<CustomerRecord> customerRecords; // 儲存用戶記錄的 vector
std::vector<TransactionRecord> transactionHistory; // 儲存各種事件記錄的 vector

void storeGiftData() // 加載初始數據
{
    #ifdef DEBUG_ENABLED
    std::cout << "GiftData Loaded" << '\n';
    #endif

    /* Push back 是用來把數據放在/加入到 vector 的最後值 */
    giftRecords.push_back({ "A01", "LG Internet TV", 3900, 19000 });
    giftRecords.push_back({ "A02", "Pioneer Hifi Set", 2400, 11500 });
    giftRecords.push_back({ "A03", "Sony DVD Player", 400, 2000 });
    giftRecords.push_back({ "B01", "Healthy Air Fryer", 1500, 7300 });
    giftRecords.push_back({ "B02", "Tefal Microwave Oven", 480, 2400 });
    giftRecords.push_back({ "B03", "Famous Coffee Maker", 1800, 8800 });
    giftRecords.push_back({ "B04", "Smart Rice Cooker", 600, 2900 });
    giftRecords.push_back({ "B05", "TechCook Toaster Oven", 450, 2250 });
    giftRecords.push_back({ "C01", "Wellcome $50 Coupon", 50, 250 });
    giftRecords.push_back({ "C02", "Mannings $50 Coupon", 50, 250 });
    giftRecords.push_back({ "C03", "Carol Restaurant $100 Coupon", 100, 500 });
    giftRecords.push_back({ "C04", "Shell $200 Coupon", 200, 960 });
    giftRecords.push_back({ "D01", "Clever Headset", 350, 1750 });
    giftRecords.push_back({ "D02", "HP Optic Mouse", 250, 1250 });
    giftRecords.push_back({ "D03", "Stylish Bluetooth Speaker", 800, 3900 });
}

void storeCustomerData() // 加載用戶數據
{
    #ifdef DEBUG_ENABLED
    std::cout << "CustomerData Loaded" << '\n';
    #endif

    customerRecords.push_back({ "Tommy2015", 'B', 8500 });
    customerRecords.push_back({ "DavidChan", 'B', 22800 });
    customerRecords.push_back({ "Luna123", 'B', 650 });
    customerRecords.push_back({ "TigerMan", 'B', 14000 });
    customerRecords.push_back({ "Max5678", 'S', 2580 });
    customerRecords.push_back({ "Neo2000", 'S', 8000 });
    customerRecords.push_back({ "CCTang", 'S', 33554 });
    customerRecords.push_back({ "EchoWong", 'G', 8650 });
    customerRecords.push_back({ "Rubychow", 'G', 28000 });
    customerRecords.push_back({ "Ivy2023", 'G', 12340 });
}

void storeInitialCustomerData() // 加載用戶數據 由於直接複製 vector (vector.assign) 會出現錯誤
{
    #ifdef DEBUG_ENABLED
    std::cout << "initialCustomerData Loaded" << '\n';
    #endif

    initialCustomerRecords.push_back({ "Tommy2015", 8500 });
    initialCustomerRecords.push_back({ "DavidChan", 22800 });
    initialCustomerRecords.push_back({ "Luna123", 650 });
    initialCustomerRecords.push_back({ "TigerMan", 14000 });
    initialCustomerRecords.push_back({ "Max5678", 2580 });
    initialCustomerRecords.push_back({ "Neo2000", 8000 });
    initialCustomerRecords.push_back({ "CCTang", 33554 });
    initialCustomerRecords.push_back({ "EchoWong", 8650 });
    initialCustomerRecords.push_back({ "Rubychow", 28000 });
    initialCustomerRecords.push_back({ "Ivy2023", 12340 });
}

void addCustomer(const std::string& id, char rank, int points) // 增加用戶數據的函數
{
    #ifdef DEBUG_ENABLED
    std::cout << "addCustomer Function Called" << '\n';
    Sleep(2000);
    #endif

    customerRecords.push_back({ id, rank, points });
    initialCustomerRecords.push_back({ id, points }); // 如果新增客戶時也新增到初始客戶數據 vector 否則初始數據將不會包含新用戶
}

void deleteCustomer(const std::string& id) // 刪除用戶數據的函數
{
    #ifdef DEBUG_ENABLED
    std::cout << "deleteCustomer Function Called" << '\n';
    Sleep(3000);
    #endif

    for (auto it{ customerRecords.begin() }; it != customerRecords.end();) // 尋找數據
    {
        if (it->customerID == id)
        {
            customerRecords.erase(it);

            break; // 如果找到則結束 loop
        }
        else
        {
            ++it; // 找不到則繼續
        }
    }
}

bool findCustomer(const std::string id) // 尋找用戶並返回是否存在用戶 use for [3] edit customer , [4] enter customer view and [5] show transaction history
{
    bool found{ false };

    for (const auto& customer : customerRecords) // For each loop (god i love for each loop)
    {
        if (customer.customerID == id)
        {
            found = true;

            break;
        }
    }
    return found;
}

bool isLeapYear(int year) // 檢查是否 leap year 的函數
{
    #ifdef DEBUG_ENABLED
    std::cout << "isLeapYear Function Called" << '\n';
    Sleep(3000);
    #endif

    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isValidDate(int day, int month, int year, const std::tm& currentTime) // 檢查是否合乎格式的日期
{
    #ifdef DEBUG_ENABLED
    std::cout << "isValidDate Function Called" << '\n';
    Sleep(3000);
    #endif

    int currentYear{ currentTime.tm_year + 1900 };
    int currentMonth{ currentTime.tm_mon + 1 };
    int currentDay{ currentTime.tm_mday };

    // 檢查是否爲未來

    if (year <= 1970)
    {
        return false;
    }

    if (year > currentYear || (year == currentYear && month > currentMonth) || (year == currentYear && month == currentMonth && day > currentDay))
    {
        #ifdef DEBUG_ENABLED
        std::cout << "Future Check Failed" << '\n';
        Sleep(3500);
        #endif

        return false;
    }

    // 檢查日期有效性

    if (month < 1 || month > 12 || day < 1) // 檢查是否超出範圍
    {
        #ifdef DEBUG_ENABLED
        std::cout << "Month Check Failed" << '\n';
        Sleep(3500);
        #endif

        return false;
    }

    // 2月份的麻煩嘢

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (isLeapYear(year))
    {
        daysInMonth[1] = 29;
    }

    if (day > daysInMonth[month - 1])
    {
        #ifdef DEBUG_ENABLED
        std::cout << "daysInMonth Check Failed" << '\n';
        Sleep(3500);
        #endif

        return false;
    }

    return true;
}

long daysDifference(const std::tm& creationTime, const std::tm& currentTime) // 計算與今天相差的日數
{
    // Convert std::tm to std::time_t

    std::time_t creationTimeEpoch = std::mktime(const_cast<std::tm*>(&creationTime));
    std::time_t currentTimeEpoch = std::mktime(const_cast<std::tm*>(&currentTime));

    // Removed cuz this wont happen since we added the exact same thing in valid date check

    /* if (creationTimeEpoch == static_cast<std::time_t>(-1) || currentTimeEpoch == static_cast<std::time_t>(-1))
    {
        std::cerr << "mktime error: year before 1900 is not accepted." << std::endl;

        Sleep(2000);

        system("cls");

        return -1;
    } */

    double diffSeconds = difftime(currentTimeEpoch, creationTimeEpoch);
    long diffDays = static_cast<long>(diffSeconds / (60 * 60 * 24));

    return diffDays;
}

long calculateDaysSinceCreation(int day, int month, int year) // 也是計算與今天相差的日數
{
    // Initialize the creation time structure

    std::tm creationTime = {};
    creationTime.tm_year = year - 1900; // tm_year is years since 1900
    creationTime.tm_mon = month - 1;    // tm_mon is 0-11
    creationTime.tm_mday = day;

    // Get the current time as std::tm
    std::time_t now = std::time(nullptr); //NULLPTR = 空指針
    std::tm currentTime;
    localtime_s(&currentTime, &now);

    // Calculate the difference in days
    return daysDifference(creationTime, currentTime);
}

char calculateRank(long days) // 根據日子返回用戶應該獲得的等級 Rank
{
    #ifdef DEBUG_ENABLED
    std::cout << "calculateRank Function Called" << '\n';
    std::cout << "ReceivedData (Days): " << days << '\n';
    Sleep(3000);
    #endif

    char userRank{ 0 };

    if (days >= 365) // 大過或等於一年
    {
        userRank = 'G';
    }
    else if (days >= 180) // 大過或等於半年
    {
        userRank = 'S';
    }
    else // 其他情況 (小於半年)
    {
        userRank = 'B';
    }

    // #ifdef DEBUG_ENABLED
    SetConsoleColor(14);
    std::cout << "User Rank is: " << userRank << '\n';
    SetConsoleColor(7);
    Sleep(2000);
    // #endif

    return userRank;
}

void errorInput() // 如果用戶輸入非數值
{
    #ifdef DEBUG_ENABLED
    std::cout << "errorInput Function Called" << '\n';
    Sleep(2000);
    #endif

    SetConsoleColor(12);
    std::cout << "Invalid input. Please enter again: ";
    SetConsoleColor(7);

    std::cin.clear();
    std::cin.ignore(1000000, '\n');
}

void updateCustomer(const std::string& id)
{
    #ifdef DEBUG_ENABLED
    std::cout << "updateCustomer Function Called" << '\n';
    #endif

    if (findCustomer(id)) // 如果 ID 和已有數據重複則刪除數據
    {
        bool sure{ areYouSure("dont clear", "delete", "dont check", id) };

        if (sure)
        {
            deleteCustomer(id);

            SetConsoleColor(10);
            std::cout << "Customer " << id << " has been removed.\n";
            SetConsoleColor(7);

            Sleep(2000);

            system("cls");
        }
        else
        {
            std::cout << "Action Canceled. Returning to menu." << '\n';

            Sleep(2000);

            system("cls");
        }

    }
    else // 如果沒有則添加數據
    {
        std::cout << "Enter the points balance for the new customer: ";

        int points{ 0 };

        int retries{ 3 };

        while ((!(std::cin >> points)) || points < 0) // 防止用戶輸入非數字或負數
        {
            retries--;

            std::cout << "You have " << retries << " retries left." << '\n';

            errorInput();

            if (retries <= 0)
            {
                SetConsoleColor(12);
                std::cout << "Run out of attempts. Returning to main menu." << '\n';
                SetConsoleColor(7);

                Sleep(2000);

                system("cls");

                return;
            }
        }

        SetConsoleColor(10);
        std::cout << "You have input a number: " << points << '\n';
        SetConsoleColor(7);

        std::cin.clear();
        std::cin.ignore(1000000, '\n');

        // Ask for date

        Retries: // goto

        std::cout << "Enter your account creation date";

        SetConsoleColor(10);
        
        std::cout << " (DD/MM/YYYY)";

        SetConsoleColor(7);
        
        std::cout << ": ";

        std::string creationDate{};

        std::getline(std::cin >> std::ws, creationDate); // Bro who use std::cin to a string and std::ws best thing

        Sleep(500);

        int day{ 0 };
        int month{ 0 };
        int year{ 0 };

        // 獲取日期

        std::istringstream dateStream(creationDate);

        char slash{};

        if (dateStream >> day >> slash >> month >> slash >> year && slash == '/') // 將 string 轉爲數字
        {
            std::time_t now = std::time(nullptr); // nullptr cool
            std::tm localTime;
            localtime_s(&localTime, &now); // Use localtime_s for thread safety

            if (isValidDate(day, month, year, localTime))
            {
                long daysSinceCreation = calculateDaysSinceCreation(day, month, year);

                if (daysSinceCreation >= 0) // Check that daysDifference didn't return an error (EXTRA SAFETY XD)
                {
                    addCustomer(id, calculateRank(daysSinceCreation), points);

                    SetConsoleColor(10);
                    std::cout << "Customer " << id << " has been added.\n";
                    SetConsoleColor(7);

                    std::cout << "Action Completed. Returning to menu." << '\n';

                    Sleep(2000);

                    system("cls");
                }
            }
            else
            {
                SetConsoleColor(12);
                std::cout << "Invalid date! It is either you have input an invalid date or it is before or equals to 1970." << '\n';
                SetConsoleColor(7);

                retries--;

                if (retries <= 0)
                {
                    SetConsoleColor(12);
                    std::cout << "Run out of attempts. Returning to main menu." << '\n';
                    SetConsoleColor(7);

                    Sleep(2000);

                    system("cls");

                    return;
                }

                std::cout << "You have " << retries << " retries left." << '\n';

                Sleep(2000);

                goto Retries; // 使用 goto (not recommended but it is so convenient)
            }
        }
        else
        {
            SetConsoleColor(12);
            std::cout << "Invalid input for date format. Please retry." << '\n';
            SetConsoleColor(7);

            retries--;

            if (retries <= 0)
            {
                SetConsoleColor(12);
                std::cout << "Run out of attempts. Returning to main menu." << '\n';
                SetConsoleColor(7);

                Sleep(2000);

                system("cls");

                return;
            }

            std::cout << "You have " << retries << " retries left." << '\n';

            Sleep(2000);

            goto Retries; // 使用 goto (not recommended but it is so convenient)
        }
    }
}

void printGiftRecord() // print gift records
{
    SetConsoleColor(10);
    std::cout << "***** Gift Records *****" << '\n';
    SetConsoleColor(7);

    std::cout << std::setfill('=') << std::setw(72) << "=" << std::setfill(' ') << std::endl;

    std::cout << std::left; // 設置左對齊 (其實無用)

    std::cout << std::setw(10) << "Gift ID"
        << std::setw(30) << "Gift Description"
        << std::setw(15) << "Price (HKD)"
        << std::setw(30) << "Points Required" << '\n';
    std::cout << std::setfill('=') << std::setw(72) << '=' << std::setfill(' ') << std::endl; // 打印分隔線

    for (auto& giftRecord : giftRecords) // 打印所有禮品記錄 using for each loop
    {
        std::cout << std::setw(10) << giftRecord.giftID
            << std::setw(30) << giftRecord.giftDescription
            << std::setw(18) << giftRecord.giftPrice
            << std::setw(22) << giftRecord.giftPointsRequired << std::left << '\n';
    }

    std::cout << std::setfill('=') << std::setw(72) << '=' << std::setfill(' ') << std::endl << '\n';
}

void printCustomerRecord() // print customer records
{
    SetConsoleColor(10);
    std::cout << "***** Customer Records *****" << '\n';
    std::cout << "Record has been printed by the order of number > capital > smaller" << '\n';
    SetConsoleColor(7);

    std::cout << std::setfill('=') << std::setw(90) << '=' << std::setfill(' ') << std::endl;

    std::cout << std::setw(50) << "CustomerID" << '\t'
        << std::setw(15) << "Rank" << '\t'
        << std::setw(20) << "Points Balance" << '\n';

    std::cout << std::setfill('=') << std::setw(90) << '=' << std::setfill(' ') << std::endl;

    // Sorting customerRecords by customerID

    std::sort(customerRecords.begin(), customerRecords.end(), [](const CustomerRecord& a, const CustomerRecord& b)
        {
            return a.customerID < b.customerID;
        });

    for (auto& customerRecord : customerRecords) // 打印所有顧客記錄 using for each loop
    {
        std::cout << std::setw(50) << customerRecord.customerID << '\t'
            << std::setw(15) << customerRecord.customerRank << '\t'
            << std::setw(20) << ((customerRecord.pointsBalance < 0) ? '\0' : customerRecord.pointsBalance) << '\n';
    }

    std::cout << std::setfill('=') << std::setw(90) << '=' << std::setfill(' ') << '\n';

    std::cout << std::endl;
}

void printRecords() // Cancer code don't bother (Not cancer anymore :D)
{
    system("cls");

    #ifdef DEBUG_ENABLED
    std::cout << "printRecords Function Called" << '\n';
    #endif

    std::cout << std::endl;

    printGiftRecord();

    printCustomerRecord();
}

auto findUserData(const std::string& customerID, const std::string& dataType, int messageOption) // 尋找用戶數據
{
    #ifdef DEBUG_ENABLED
    std::cout << "findUserData Function Called" << '\n';
    Sleep(2000);
    #endif

    for (auto& customerRecord : customerRecords)
    {
        if (findCustomer(customerID) == 1 && customerRecord.customerID == customerID)
        {
            if (dataType == "point")
            {
                if (messageOption == 1)
                {
                    SetConsoleColor(14);
                    std::cout << "Customer " << customerID << " currently have " << ((customerRecord.pointsBalance < 0) ? '\0' : customerRecord.pointsBalance) << " points." << '\n';
                    SetConsoleColor(7);
                }

                return customerRecord.pointsBalance;
            }

            if (dataType == "rank")
            {
                if (messageOption == 1)
                {
                    SetConsoleColor(14);
                    std::cout << "Customer " << customerID << " currently is at Rank " << customerRecord.customerRank << '\n';
                    SetConsoleColor(7);

                    if (customerRecord.customerRank == 'G')
                    {
                        SetConsoleColor(10);
                        std::cout << "You can have 10% off discount for redeeming gifts!" << '\n';
                        SetConsoleColor(7);
                    }
                    else if (customerRecord.customerRank == 'S')
                    {
                        SetConsoleColor(10);
                        std::cout << "You can have 5% off discount for redeeming gifts!" << '\n';
                        SetConsoleColor(7);
                    }
                    else
                    {
                        SetConsoleColor(12);
                        std::cout << "You don't have any discount on your rank! Level Up to receive extra advantages!" << '\n';
                        SetConsoleColor(7);
                    }

                    std::cout << std::endl;
                }

                return static_cast<int>(customerRecord.customerRank); // 將 char 變爲 int 否則 auto data type 將無法運行
            }
        }
    }

    if (messageOption == 1)
    {
        SetConsoleColor(10);
        std::cout << "Action Completed." << '\n';
        SetConsoleColor(7);

        std::cout << "Backing main menu." << '\n';
    }

    Sleep(2000);

    return 0;
}

auto findInitialData(const std::string& customerID)
{
    #ifdef DEBUG_ENABLED
    std::cout << "Function findInitialData Called" << '\n';
    #endif

    for (auto& initialCustomerRecord : initialCustomerRecords)
    {
        if (initialCustomerRecord.initialCustomerID == customerID)
        {
            return initialCustomerRecord.initialPointsBalance;
        }
    }

    SetConsoleColor(12);
    std::cout << "Customer " << customerID << " not found in initial records." << '\n';
    SetConsoleColor(7);
    return 0;
}

std::vector<GiftRecord> getAvailableGiftsForRedemption(const std::string& customerID, double discount = 1.0)
{
    // 過濾並排序禮品記錄
    std::vector<GiftRecord> availableGifts;

    int customerPoints{ findUserData(customerID, "point", 0) };

    // 先將所有禮品記錄添加到臨時vector中

    for (const auto& gift : giftRecords)
    {
        if (gift.giftPointsRequired * discount <= customerPoints)
        {
            availableGifts.push_back(gift);
        }
    }

    // 根據所需積分對可用禮品進行排序
    std::sort(availableGifts.begin(), availableGifts.end(), [](const GiftRecord& a, const GiftRecord& b)
    {
            return a.giftPointsRequired < b.giftPointsRequired;
    });

    return availableGifts;
}

void earnCCPoint(const std::string& customerID, double moneyAmount)
{
    #ifdef DEBUG_ENABLED
    std::cout << "earnCCPoint Function Called" << '\n';
    Sleep(2000);
    #endif

    int newPoints = static_cast<int>(floor(moneyAmount / 250));

    double extraMoney{};

    for (auto& extraMoneys : transactionHistory)
    {
        if (findCustomer(customerID) == 1 && extraMoneys.customerID == customerID)
        {
            extraMoney = extraMoneys.extraMoney;
        }
    }

    for (auto& ccPoints : customerRecords)
    {
        if (findCustomer(customerID) == 1 && ccPoints.customerID == customerID)
        {
            if (ccPoints.pointsBalance < 0)
                ccPoints.pointsBalance = 0;

            ccPoints.pointsBalance += newPoints;

            // #ifdef DEBUG_ENABLED
            SetConsoleColor(10);
            std::cout << "Your Current Points is: " << ccPoints.pointsBalance << '\n';
            // #endif

            std::cout << "Customer " << customerID << " ccPoints has been updated!" << '\n';
            SetConsoleColor(7);
            Sleep(1500);
            transactionHistory.push_back({ customerID, "Earn CC Points", std::to_string(newPoints), newPoints, extraMoney });

            return;
        }
    }

    std::cout << "Error 420: User Not Found." << '\n'; // Impossible since we already have a findCustomer function
}

void reduceCCPoint(const std::string& customerID, int newPoints) // 減分 兌換禮品時使用
{
    for (auto& ccPoints : customerRecords)
    {
        if (findCustomer(customerID) == 1 && ccPoints.customerID == customerID)
        {
            ccPoints.pointsBalance -= newPoints;

            #ifdef DEBUG_ENABLED
            std::cout << "Current Points: " << ccPoints.pointsBalance << '\n';
            #endif

            SetConsoleColor(10);
            std::cout << "Customer " << customerID << " ccPoints has been updated!" << '\n';
            SetConsoleColor(7);
        }
    }
    return;
}

void printByCategory(std::string username, char giftCategory, double discount);

void redeemGift(CustomerRecord& customer, double discount, char allowedCategory, std::string customerID) // 兌換禮品的函數
{
    int attempts{ 3 };

    const int maxAttempts{ 0 };

    bool success{ 0 };

    while (attempts + 2 > maxAttempts && !success)
    {
        if (attempts <= maxAttempts)
        {
            SetConsoleColor(12);
            std::cout << "Failed to redeem gift after " << maxAttempts << " attempts. Returning to customer menu." << '\n';
            SetConsoleColor(7);
            Sleep(2500);
            break;
        }

        std::cout << "Enter the giftID that you want to redeem ";

        SetConsoleColor(12);
        std::cout << "(q to exit)";
        SetConsoleColor(7);

        std::cout << ": ";

        std::string giftID{};

        std::getline(std::cin >> std::ws, giftID);

        if (giftID == "q" || giftID == "Q") // if 用戶想取消
        {
            system("cls");

            std::cout << "Backing Customer View Menu." << '\n';

            Sleep(1000);

            return;
        }

        std::regex giftIDPattern("^[A-D][0-9]{2}$"); // 限制為一個字母（A-D）和兩個數字

        if (!std::regex_match(giftID, giftIDPattern) || giftID[0] != allowedCategory)
        {
            SetConsoleColor(12);
            std::cout << "Invalid gift category or format. Please enter a gift ID from the correct category and format. " << '\n';
            SetConsoleColor(7);

            attempts--;

            std::cout << "You have " << attempts << " attempts left." << '\n';

            continue;
        }

        auto it = std::find_if(giftRecords.begin(), giftRecords.end(), [&giftID](const GiftRecord& gift)
        {
                return gift.giftID == giftID;
        });

        if (it == giftRecords.end())
        {
            SetConsoleColor(12);
            std::cout << "Invalid Gift ID. Please try again." << '\n';
            SetConsoleColor(7);

            attempts--;

            std::cout << "You have " << attempts << " attempts left." << '\n';

            continue;
        }

        double extraMoney{};

        for (auto& extraMoneys : transactionHistory)
        {
            if (findCustomer(customerID) == 1 && extraMoneys.customerID == customerID)
            {
                extraMoney = extraMoneys.extraMoney;
            }
        }

        int pointsNeeded{ static_cast<int>(it->giftPointsRequired * discount) };

        if (customer.pointsBalance < pointsNeeded)
        {
            SetConsoleColor(12);
            std::cout << "Your CC point is not enough for redeeming this gift. Extra money is required." << '\n';
            SetConsoleColor(7);

            if (customer.pointsBalance < 0)
                customer.pointsBalance = 0;

            #ifdef DEBUG_ENABLED
            std::cout << "exMon: " << extraMoney << '\n';
            std::cout << "ptNeeded: " << pointsNeeded << '\n';
            std::cout << "cusPtbal: " << customer.pointsBalance << '\n';
            #endif

            SetConsoleColor(12);
            std::cout << "If you sure, you now need pay $" << ((pointsNeeded - customer.pointsBalance) * 0.2) + extraMoney << " extra money for all your current redeemed gift." << '\n';
            SetConsoleColor(7);

            bool sure{ areYouSure("dont clear", "NULL", "NULL", "NULL") };

            if (!sure)
                continue;
        }

        if (customer.pointsBalance < pointsNeeded)
            extraMoney = (((pointsNeeded - customer.pointsBalance) * 0.2) + extraMoney);

        #ifdef DEBUG_ENABLED
        std::cout << "point bal: " << customer.pointsBalance << '\n';
        std::cout << "extra money: " << extraMoney << '\n';
        #endif

        transactionHistory.push_back({ customerID, "Redeem Gift", "Gift ID: " + it->giftID + ", Gift Description: " + it->giftDescription + ", Actual Points Used: " + ((pointsNeeded < customer.pointsBalance) ? std::to_string(pointsNeeded) : std::to_string(customer.pointsBalance)), -pointsNeeded, extraMoney});

        reduceCCPoint(customer.customerID, pointsNeeded);

        SetConsoleColor(10);
        std::cout << "Redemption successful! You have redeemed " << it->giftDescription

            << " and your remaining points are " << ((customer.pointsBalance < 0) ? '\0' : customer.pointsBalance) << ".\n";
        SetConsoleColor(7);

        Sleep(3500);

        success = true;

        if (!success) // run out of attempts then 返回
        {
            SetConsoleColor(12);
            std::cout << "Failed to redeem gift after " << maxAttempts << " attempts. Returning to customer menu." << '\n';
            SetConsoleColor(7);
            Sleep(2500);
            break;
        }
        else
        {
            printByCategory(customer.customerID, allowedCategory, discount);
        }
    }

    return;
}

void printByCategory(std::string username, char giftCategory, double discount) // 剩係 print 某個種類嘅禮品
{
    system("cls");

    findUserData(username, "point", 1);

    findUserData(username, "rank", 1);

    SetConsoleColor(10);
    std::cout << "***** Filtered Gift Records *****" << '\n';
    SetConsoleColor(7);
    std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl;

    std::cout << std::left; // 設置左對齊

    std::cout << std::setw(10) << "Gift ID"
        << std::setw(30) << "Gift Description"
        << std::setw(18) << "Price (HKD)"
        << std::setw(20) << "Points Required";
        
    SetConsoleColor(10);
    std::cout << std::setw(30) << "Points Required (After Discount)" << '\n';
    SetConsoleColor(7);

    std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl; // 打印分隔線

    for (const auto& giftRecord : giftRecords)
    {
        if (giftRecord.giftID[0] == giftCategory)
        {
            double discountedPoint = giftRecord.giftPointsRequired * (discount);

            std::cout << std::setw(10) << giftRecord.giftID
                << std::setw(30) << giftRecord.giftDescription
                << std::setw(18) << giftRecord.giftPrice
                << std::setw(20) << giftRecord.giftPointsRequired;

            SetConsoleColor(10);
            std::cout << std::setw(30) << std::fixed << std::setprecision(2) << static_cast<int>(round(discountedPoint)) << '\n';
            SetConsoleColor(7);
        }
    }

    std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl << '\n';
    SetConsoleColor(10);
    std::cout << "***** Available Gift Redeem *****" << '\n';
    SetConsoleColor(7);
    std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl;

    std::vector<GiftRecord> redeemableGifts = getAvailableGiftsForRedemption(username, discount);

    bool hasRedeemableGifts = false;

    for (const auto& gift : redeemableGifts)
    {
        if (gift.giftID[0] == giftCategory)
        {
            hasRedeemableGifts = true;
            break;
        }
    }

    if (!hasRedeemableGifts)
    {
        SetConsoleColor(12);
        std::cout << "You have no available gifts in category " << giftCategory << " that can be redeemed only through CC Points." << '\n';
        std::cout << "You will have to use extra money to redeem gifts in this category." << '\n';
        SetConsoleColor(7);
        std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl;
    }
    else
    {
        std::cout << std::setw(10) << "Gift ID"
            << std::setw(30) << "Gift Description"
            << std::setw(18) << "Price (HKD)"
            << std::setw(20) << "Points Required";

        SetConsoleColor(10);
        std::cout << std::setw(30) << "Points Required (After Discount)" << '\n';
        SetConsoleColor(7);

        std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl;

        for (const auto& gift : redeemableGifts)
        {
            if (gift.giftID[0] == giftCategory)
            {
                double discountedPoint = gift.giftPointsRequired * (discount);
                std::cout << std::setw(10) << gift.giftID
                    << std::setw(30) << gift.giftDescription
                    << std::setw(18) << gift.giftPrice
                    << std::setw(20) << gift.giftPointsRequired;

                SetConsoleColor(10);
                std::cout << std::setw(30) << std::fixed << std::setprecision(2) << static_cast<int>(round(discountedPoint)) << '\n';
                SetConsoleColor(7);
            }
        }

        std::cout << std::setfill('=') << std::setw(120) << '=' << std::setfill(' ') << std::endl;
    }

    // redeemGift(giftRecords, discount)

    auto it = std::find_if(customerRecords.begin(), customerRecords.end(), [&username](const CustomerRecord& customer)
    {
        return customer.customerID == username;
    });

    if (it != customerRecords.end())
    {
        redeemGift(*it, discount, giftCategory, username);
    }
    else
    {
        SetConsoleColor(12);
        std::cout << "Customer record not found." << '\n';
        SetConsoleColor(7);
    }
}

void sortByCategory(std::string username, char category, char rank)
{
    char giftCategory{ 0 };
    double discount{ 1.0 };

    if (category == '1')
    {
        giftCategory = 'A';
    }
    else if (category == '2')
    {
        giftCategory = 'B';
    }
    else if (category == '3')
    {
        giftCategory = 'C';
    }
    else if (category == '4')
    {
        giftCategory = 'D';
    }
    else
    {
        return;
    }

    if (rank == 'G')
    {
        discount = 0.9;
    }
    else if (rank == 'S')
    {
        discount = 0.95;
    }
    else
    {
        discount = 1;
    }

    printByCategory(username, giftCategory, discount);
}

void editCCPoint(const std::string& customerID, int newPoints) // R4 enter customer view [3] modify cc point balance
    #ifdef DEBUG_ENABLED
    std::cout << "editCCPoint Function Called" << '\n';
    Sleep(2000);
    #endif

    double extraMoney{};

    for (auto& extraMoneys : transactionHistory)
    {
        if (findCustomer(customerID) == 1 && extraMoneys.customerID == customerID)
        {
            extraMoney = extraMoneys.extraMoney;
        }
    }

    for (auto& ccPoints : customerRecords)
    {
        if (findCustomer(customerID) == 1 && ccPoints.customerID == customerID)
        {
            if (ccPoints.pointsBalance < 0)
                ccPoints.pointsBalance = 0;

            int pointsChange = newPoints - ccPoints.pointsBalance;

            ccPoints.pointsBalance = newPoints;

            #ifdef DEBUG_ENABLED
            std::cout << "Current Points: " << ccPoints.pointsBalance << '\n';
            #endif

            std::cout << "Customer " << customerID << " ccPoints has been edited!" << '\n';

            std::string changeType = pointsChange >= 0 ? "Increase" : "Decrease";

            // std::cout << "pt changed: " << pointsChange << '\n';

            transactionHistory.push_back({ customerID, "Modify CC Points", changeType + " CC Points by " + std::to_string(std::abs(pointsChange)), pointsChange, extraMoney });

            return;
        }
    }

    std::cout << "Error 404: User Not Found." << '\n';
}

void convertCCPoint(const std::string& customerID, double moneyAmount) // R4 enter customer view [1] earn cc point
{
    #ifdef DEBUG_ENABLED
    std::cout << "convertCCPoint Function Called" << '\n';
    Sleep(2000);
    #endif

    char userRank{ static_cast<char>(findUserData(customerID, "rank", 0)) };

    if (findCustomer(customerID))
    {
        if (moneyAmount < 250)
        {
            std::cout << "No CC point can be earned by the remaining spending less than $250." << '\n';

            Sleep(1500);

            system("cls");

            return;
        }
        else
        {
            #ifdef DEBUG_ENABLED
            std::cout << "Money: " << floor(moneyAmount / 250) << '\n';
            #endif

            earnCCPoint(customerID, floor(moneyAmount));

            Sleep(2000);
        }
    }
}

char customerViewInput(std::string username) // [4] Customer View 的界面
{
    #ifdef DEBUG_ENABLED
    std::cout << "customerViewInput Function Called" << '\n';
    Sleep(2000);
    #endif

    char userInput{ 0 };

    do
    {
        system("cls");
        
        SetConsoleColor(10);
        std::cout << "Action for Customer ID: " << username << '\n';
        SetConsoleColor(7);
        std::cout << "***** Customer View Menu *****" << '\n';
        std::cout << "[1] Earn CC Points" << '\n';
        std::cout << "[2] Redeem Gifts" << '\n';
        std::cout << "[3] Modify CC Points Balance" << '\n';
        std::cout << "[4] Return to Main Menu" << '\n';
        std::cout << "**************************" << '\n';
        std::cout << "Option (1 - 4): ";

        std::cin >> userInput;

        if (std::cin.peek() == '\n')
        {
            std::cin.ignore(10000, '\n');

            if (userInput >= '1' && userInput <= '4')
            {
                break;
            }
        }
        else
        {
            SetConsoleColor(12);
            std::cout << "Invalid input. Please enter exactly one character.\n";
            SetConsoleColor(7);

            std::cin.clear();

            std::cin.ignore(10000, '\n');
        }

    } while (true);

    system("cls");

    return userInput;
}

void showTransactionHistory(const std::string& customerID) // [5] Show Transaction History 的函數
{
    int originalPoints{ findInitialData(customerID) };

    // int realFinalPoints{ findUserData(customerID, "point", "NULL", 0) };

    int finalPoints{ findUserData(customerID, "point", 0) };

    if (finalPoints < 0)
        finalPoints = 0;

    int totalPointsChange{ finalPoints - originalPoints };

    double totalExtraMoney{ 0 };

    double moneyToPoint{ 0 };

    double redeemExtraMoney{ 0 };

    std::cout << std::endl;

    SetConsoleColor(10);
    std::cout << "***** Transaction History *****" << '\n';
    SetConsoleColor(7);

    std::cout << std::setfill('=') << std::setw(80) << '=' << std::setfill(' ') << std::endl;

    for (const auto& transaction : transactionHistory)
    {
        if (transaction.customerID == customerID)
        {
            std::cout << "Type: " << transaction.type << '\n';

            if (transaction.type == "Earn CC Points")
            {
                SetConsoleColor(10);
                std::cout << "Points Earned: " << "+" << transaction.details << '\n';
                SetConsoleColor(7);
            }
            else
            {
                std::cout << "Details: ";
                if (transaction.type == "Redeem Gift")
                    SetConsoleColor(14);
                else
                    SetConsoleColor(7);
                std::cout << transaction.details << '\n';
            }

            if (transaction.type == "Modify CC Points")
            {
                if (transaction.pointsChange <= 0)
                {
                    if (transaction.pointsChange == 0)
                        SetConsoleColor(7);
                    else
                        SetConsoleColor(12);
                    std::cout << "Points Changed: " << transaction.pointsChange << '\n';
                    SetConsoleColor(7);
                }
                else
                {
                    SetConsoleColor(10);
                    std::cout << "Points Changed: " << "+" << transaction.pointsChange << '\n';
                    SetConsoleColor(7);
                }
            }
            else
            {
                if (transaction.type != "Earn CC Points")
                {
                    SetConsoleColor(12);
                    std::cout << "Actual Points Charged: " << transaction.pointsChange << '\n';
                    SetConsoleColor(7);
                }
            }

            if (transaction.type == "Earn CC Points")
                std::cout << "Money Used On Earn CC Point: $" << transaction.pointsChange * 250 << '\n';

            if (transaction.type == "Redeem Gifts")
            {
                SetConsoleColor(12);
                std::cout << "Extra Money: $" << transaction.extraMoney << '\n';
                SetConsoleColor(7);
            }

            std::cout << std::setfill('-') << std::setw(80) << '-' << std::setfill(' ') << std::endl;

            redeemExtraMoney = transaction.extraMoney;
        }
    }

    std::cout << "Original CC Points Balance: " << originalPoints << '\n';

    std::cout << "Final CC Points Balance: " << finalPoints << '\n';

    if (totalPointsChange <= 0)
    {
        if (totalPointsChange == 0)
            SetConsoleColor(7);
        else
            SetConsoleColor(12);
        std::cout << "Total Change in CC Points: " << totalPointsChange << '\n';
        SetConsoleColor(7);
    }
    else
    {
        SetConsoleColor(10);
        std::cout << "Total Change in CC Points: " << "+" << totalPointsChange << '\n';
        SetConsoleColor(7);
    }

    if (redeemExtraMoney == 0)
        SetConsoleColor(10);
    else
        SetConsoleColor(12);

    std::cout << "Total Missing Points: " << static_cast<int>(redeemExtraMoney / 0.2) << '\n';
    if (redeemExtraMoney > 0)
        std::cout << "After * 0.2" << '\n';
    std::cout << "Total Extra Money to Pay: $" << redeemExtraMoney << '\n';
    SetConsoleColor(7);


    std::cout << std::setfill('=') << std::setw(80) << '=' << std::setfill(' ') << std::endl;
}

void ccPointChanged(const std::string& customerID, int points)
{
    int originalPoints{ points };

    int finalPoints{ findUserData(customerID, "point", 0) };

    if (originalPoints < 0)
        originalPoints = 0;

    int totalPointsChange{ finalPoints - originalPoints };

    if (totalPointsChange < 0)
    {
        SetConsoleColor(12);
        std::cout << "Points REDUCED for " << customerID << " is " << abs(totalPointsChange) << '\n';
        SetConsoleColor(7);
    }
    else if (totalPointsChange == 0)
    {
        std::cout << "No Points has been changed. Returning to previous page." << '\n';
    }
    else
    {
        SetConsoleColor(10);
        std::cout << "Points INCREASED for " << customerID << " is " << totalPointsChange << '\n';
        SetConsoleColor(7);
    }

    Sleep(3000);
}

void creditExit() // 結束的函數
{
    #ifdef DEBUG_ENABLED
    std::cout << "creditExit Function Called" << '\n';
    #endif

    SetConsoleColor(10);
    std::cout << "********** [Credits] **********" << '\n';
    std::cout << "Rainbow name will be given to people who did something (really sth...)" << '\n';
    std::cout << std::endl;
    SetConsoleColor(7);

    // Define an array of colors for the rainbow effect
    // 有做嘢我都畀 rainbow name
    // Names are removed due to privacy

    WORD color1[] = { 12, 14, 10, 11, 13, 9 };
    WORD color2[] = { 10, 11, 13, 9, 12, 14 };
    WORD color3[] = { 13, 9, 12, 14, 10, 11 };


    Sleep(1000);

    for (int i{ 5 }; i > 0; --i)
    {
        std::cout << "(Program Exit in " << i << " Seconds)" << '\r';

        std::cout.flush();

        Sleep(1000);
    }

    system("cls");

    SetConsoleColor(7);

    return;
}

std::string askCustomerID() // 詢問用戶 customerID 方便調用
{
    #ifdef DEBUG_ENABLED
    std::cout << "askCustomerID Function Called" << '\n';
    Sleep(2000);
    #endif

    int retries{ 3 };

    //nameAgain: //goto fixing
    SetConsoleColor(12);
    std::cout << "Customer ID must be 100% matched the data (including uppercase or lowercase)" << "\n\n";
    SetConsoleColor(7);
    std::cout << "Enter Customer ID: ";

    std::string customerID{};

    std::getline(std::cin >> std::ws, customerID);

    while (customerID.length() > 50)
    {
        SetConsoleColor(12);
        std::cout << "Customer ID should not longer than 50 characters." << '\n';
        SetConsoleColor(7);

        retries--;

        std::cout << "You have " << retries << " attempts left." << '\n';

        if (retries <= 0)
        {
            SetConsoleColor(12);
            std::cout << "You have no attempts left. Returning to main menu." << '\n';
            SetConsoleColor(7);

            Sleep(2000);

            system("cls");

            return "Error";
            
        }

        std::cout << "Enter Customer ID: ";

        std::getline(std::cin >> std::ws, customerID);

        // goto nameAgain; fixing
    }

    return customerID;
}

int main()
{
    bool dataLoaded = false; // 檢查用戶是否已經選擇過 [1] 的布爾值

    // 初始化數據
    storeGiftData();
    storeCustomerData();
    storeInitialCustomerData();

    while (true)
    {
        char userInput{ getUserInput(dataLoaded) }; // 此處我們引用了 LoadStartingData 的函數，並把函數返回的值再次賦予到新的變量

        if (userInput == '1')
        {
            #ifndef DEBUG_ENABLED // Debug 模式時跳過選項 [1]

            if (dataLoaded)
            {
                system("cls");

                Sleep(1000);

                SetConsoleColor(10);
                std::cout << "Starting Data has been loaded already" << '\n';
                SetConsoleColor(7);

                std::cout << "Backing Main Menu" << '\n';

                Sleep(2500);

                system("cls");
            }
            else
            {
                system("cls");

                std::cout << "Loading data..." << '\n';
                SetConsoleColor(14);
                std::cout << "Do not select anything in console when progress bar is working, if you did just press ENTER" << '\n';
                SetConsoleColor(7);
                std::cout << '\n';

                Sleep(1000);

                /* 假的進度條 */

                int total{ 50 };

                for (int i{ 0 }; i <= total; ++i)
                {
                    progressBar(i, total);

                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }

                std::cout << std::endl; // 更新std::cout的緩存

                SetConsoleColor(10);

                std::cout << "Data has been successfully loaded" << '\n';

                SetConsoleColor(7);

                std::cout << "Backing Main Menu" << '\n';

                Sleep(2500);

                system("cls");

                dataLoaded = true;
            }

            #endif
        }

        if (userInput == '2')
        {
            printRecords();
        }

        if (userInput == '3')
        {
            bool sure{ areYouSure("clear", "NULL", "check", "NULL") };

            if (sure)
            {
                std::cout << "If customerID is ";
                
                SetConsoleColor(10);

                std::cout << "FOUND";

                SetConsoleColor(7);

                std::cout << ", ";

                SetConsoleColor(12);
                    
                std::cout << "DELETE";

                SetConsoleColor(7);
                
                std::cout << " customer data will be in action." << '\n';

                std::cout << "If customerID is ";

                SetConsoleColor(12);
                
                std::cout << "NOT FOUND";

                SetConsoleColor(7);
                
                std::cout << ", ";
                
                SetConsoleColor(10);

                std::cout << "CREATE";

                SetConsoleColor(7);
                
                std::cout << " customer data will be in action." << '\n';

                std::cout << std::endl;

                std::string customerID{ askCustomerID() };

                if (customerID != "Error")
                    updateCustomer(customerID);
            }
        }

        if (userInput == '4')
        {
            bool sure{ areYouSure("clear", "NULL", "check", "NULL") };

            if (sure)
            {
                char userInputCVM{ 0 };

                std::string customerID{ askCustomerID() };

                if (findCustomer(customerID) && customerID != "Error")
                {
                    customerMenu:

                    userInputCVM = customerViewInput(customerID);

                    if (userInputCVM < '0' || userInputCVM > '4')
                    {
                        std::cin.clear();

                        std::cin.ignore(10000, '\n');

                        std::cout << "Error Input Occured. Returning to main menu." << std::endl;
                    }

                    int points{ findUserData(customerID, "point", 0) };

                    if (userInputCVM == '1')
                    {
                        bool sure{ areYouSure("clear", "NULL", "NULL", "NULL") };

                        if (sure)
                        {
                            std::cout << "Enter the amount of money spent: ";

                            double moneyAmount{ 0 };

                            double epsilon{ 0.001 };

                            while (!(std::cin >> moneyAmount) || moneyAmount > 999999)
                            {
                                if (moneyAmount > 999999)
                                {
                                    SetConsoleColor(12);
                                    std::cout << "The range should between 0 to 999999" << '\n';
                                    SetConsoleColor(7);
                                }

                                errorInput();
                            }

                            SetConsoleColor(10);
                            std::cout << "You have input a number: " << moneyAmount << '\n';
                            SetConsoleColor(7);

                            Sleep(1500);

                            if (moneyAmount > 249.999 && moneyAmount < 250)
                                moneyAmount -= epsilon;

                            convertCCPoint(customerID, moneyAmount);

                            if (points > 0)
                                ccPointChanged(customerID, points);
                        }

                        goto customerMenu;
                    }

                    if (userInputCVM == '2')
                    {
                        char userRanks{ static_cast<char>(findUserData(customerID, "rank", 1)) };

                        char userInputGC{ 0 };

                        std::string input{};

                        do
                        {
                            std::cout << "***** Gift Category *****" << '\n';
                            std::cout << "[1] Audio & Video" << '\n';
                            std::cout << "[2] Kitchenware" << '\n';
                            std::cout << "[3] Coupons" << '\n';
                            std::cout << "[4] Computer Accessories" << '\n';
                            std::cout << "[5] Return to previous page" << '\n';
                            std::cout << "[6] Main Menu" << '\n';
                            std::cout << "*****************" << '\n';
                            std::cout << "Option (1 - 6): ";

                            std::cin >> input;

                            if (input.length() == 1 && std::isdigit(input[0]) && input[0] >= '1' && input[0] <= '6')
                            {
                                userInputGC = input[0];
                            }
                            else
                            {
                                system("cls");

                                findUserData(customerID, "rank", 1);

                                userInputGC = 0; // Reset userInputGC to an invalid value
                            }

                        } while (userInputGC == 0);

                        std::cin.clear();

                        std::cin.ignore(10000, '\n');

                        sortByCategory(customerID, userInputGC, userRanks);

                        if (userInputGC != '6')
                            goto customerMenu;

                    }

                    if (userInputCVM == '3')
                    {
                        bool sure{ areYouSure("clear", "NULL", "NULL", "NULL") };

                        if (sure)
                        {
                            int points{ findUserData(customerID, "point", 1) };

                            Sleep(1000);

                            std::cout << "Enter a positive value: ";

                            int ccPointValue{ 0 };

                            while (!(std::cin >> ccPointValue) || ccPointValue < 0 || ccPointValue > 999999)
                            {
                                if (ccPointValue > 999999)
                                {
                                    SetConsoleColor(12);
                                    std::cout << "The range should between 0 to 999999" << '\n';
                                    SetConsoleColor(7);
                                }

                                errorInput();
                            }

                            SetConsoleColor(10);
                            std::cout << "You have input a number: " << ccPointValue << '\n';
                            SetConsoleColor(7);

                            Sleep(1500);

                            editCCPoint(customerID, ccPointValue);

                            ccPointChanged(customerID, points);

                            std::cin.clear();
                            std::cin.ignore(1000000, '\n');
                            std::cout << std::endl;
                        }

                        goto customerMenu;
                    }

                    system("cls");
                }
                else
                {
                    SetConsoleColor(12);
                    std::cout << "User ID not found or User ID too long. Returning to menu." << '\n';
                    SetConsoleColor(7);

                    Sleep(2000);

                    system("cls");

                    continue;
                }
            }
        }

        if (userInput == '5')
        {
            bool sure{ areYouSure("clear", "NULL", "check", "NULL") };

            if (sure)
            {
                std::string customerID{ askCustomerID() };

                if (!findCustomer(customerID) || customerID == "Error")
                {
                    SetConsoleColor(12);
                    std::cout << "User Not Found" << '\n';
                    SetConsoleColor(7);

                    std::cout << "Backing Menu" << '\n';

                    Sleep(2000);

                    system("cls");

                    continue;
                }
                else
                {
                    showTransactionHistory(customerID);
                }
            }
        }

        if (userInput == '6')
        {
            bool sure{ areYouSure("clear", "exit", "check", "NULL") };

            if (sure)
            {
                system("cls");

                creditExit();

                break;
            }
            else
            {
                std::cout << "Action Canceled. Returning to main menu." << '\n';

                Sleep(2000);

                system("cls");
            }
        }

        /* 以下皆爲偵錯時使用 */

        #ifdef DEBUG_ENABLED

        if (userInput == '7')
        {
            std::string userID(askCustomerID());

            findInitialData(userID);
        }

        if (userInput == '8')
        {
            std::string userID{ askCustomerID() };

            deleteCustomer(userID);

            Sleep(2000);

            system("cls");
        }

        if (userInput == '9')
        {
            std::cout << "[1] Get CC Point" << '\n';
            std::cout << "[2] WriteMemoryAccess CC Point" << '\n';
            std::cout << "[3] Main Menu" << '\n';

            std::cout << "Select: ";

            char select{ 0 };

            std::cin >> select;

            if (select == '3')
            {
                system("cls");

                continue;
            }

            system("cls");

            std::string name{ askCustomerID() };

            std::cout << "Points: ";

            int points{ 0 };

            std::cin >> points;

            (select == '1' ? earnCCPoint(name, points * 250) : editCCPoint(name, points));

            Sleep(2000);

            system("cls");
        }

        #endif
    }

    return 0; // 結束代碼 byebye
}
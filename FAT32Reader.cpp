// LR3_Sasha.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <string>
#include <iomanip>
using namespace std;
#pragma pack(push, 1)
typedef struct
{
    BYTE NOP[3];
    BYTE OEM[8];
    INT16 BytesInSector;
    BYTE SectorsInCluster;
    INT16 ReservedSectors;
    BYTE CopyFat;
    INT16 reserved2;
    INT16 CountSectors16;
    BYTE TypeOfCarrier;
    INT16 reserved4;
    INT16 SectorInLine;
    INT16 CountHeads;
    DWORD SectorsBeforeBegin;
    DWORD CountSectors32;
    DWORD FatInSectors;
    DWORD ClusterMainDir;
}BOOT_FAT32;
int FirstClusterOffset;
int ClusterSize;
int BytesInSector;
bool ReadCluster(HANDLE, BYTE*, int, int);

int main()
{
    setlocale(LC_ALL, "rus");
    cout << "Какой логический том нужно открыть(укажите латинскую букву)"<<endl;
    string path;
    cin>>path;
    path= "\\\\.\\" + path + ":";
    
    HANDLE OpenDisk = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ || FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    BYTE buffer[512];
    DWORD BytesToRead = sizeof(buffer), BytesReaded;
    bool ReadResult = ReadFile(OpenDisk, buffer, BytesToRead, &BytesReaded, NULL);
    BOOT_FAT32* boot = (BOOT_FAT32*)buffer;
    const BYTE sign[] = {0x4d,0x53,0x44,0x4f,0x53,0x35,0x2e,0x30 };
    for (int i = 0; i < size(sign); i++)
    {
        if (sign[i] != boot->OEM[i])
        {
            cout << "Это не FAT32!" << endl;
            exit(-1);
        }
    }
        
    ClusterSize = boot->SectorsInCluster * boot->BytesInSector;
    FirstClusterOffset = (boot->CopyFat * boot->FatInSectors + boot->ReservedSectors) * boot->BytesInSector;
    BytesInSector = boot->BytesInSector;
    cout << "Файловая система FAT32" << endl;
    cout << "Секторов в кластере " << boot->SectorsInCluster<<endl;
    cout << "Байт в секторе " << boot->BytesInSector << endl;
      cout <<"Количество байт в кластере: "<< ClusterSize << endl;
      cout << "Размер зарезервированной области: " << boot->ReservedSectors << endl;
      if (boot->CountSectors16 == 0) cout << "Количество секторов " << boot->CountSectors32 << " \tКоличество кластеров " << boot->CountSectors32
          / boot->SectorsInCluster<<endl;
      else cout << "Количество секторов " << boot->CountSectors16 << " \tКоличество кластеров " << boot->CountSectors16
          / boot->SectorsInCluster << endl;
      cout << "Размер FAT в секторах " << boot->FatInSectors << endl;
      cout << "Кластер, в котором находится корневой каталог: " << boot->ClusterMainDir<<endl;

      cout << "\n\n===================================================================\n";
      cout << "с какого кластера начать чтение?" << endl;
      int NumClus;
      cin >> NumClus;
      cout << "Сколько кластеров нужно прочитать?" << endl;
      int CountClus;
      cin >> CountClus;
      BYTE* Cluster = new BYTE[ClusterSize*CountClus];
      bool a = ReadCluster(OpenDisk, Cluster, NumClus, CountClus);
      for (int i = 0; i < ClusterSize * CountClus; ++i)
      {
          cout << std::hex << setw(2) << setfill('0') << uppercase << int(Cluster[i]) << ' ';
          if ((i + 1) % 16 == 0)
              cout << endl;
      }

      delete[] Cluster;
      CloseHandle(OpenDisk);   
}
bool ReadCluster(HANDLE DiskHandle, BYTE* Buffer, int NumberOfCluster, int CountCluster)
{
    auto res = SetFilePointer(DiskHandle, FirstClusterOffset + NumberOfCluster*ClusterSize, NULL, FILE_BEGIN);
    DWORD BytesRead;
    auto readdisk = ReadFile(DiskHandle, Buffer, ClusterSize * CountCluster, &BytesRead, NULL);
    if (!readdisk || BytesRead != ClusterSize * CountCluster) return false;
}


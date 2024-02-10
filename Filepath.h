#pragma once

//Path for video once it's downloaded
LPCWSTR videoPath = L"C:\\jakewashere\\bird.mp4";

//Path for Credit card wav once it's downloaded
LPCWSTR CreditCard = L"C:\\jakewashere\\CreditCard.wav";

//Final VLC path after install
LPCWSTR vlcPath = L"C:\\Program Files\\VideoLAN\\VLC\\vlc.exe";

//Rootpath for files
LPCWSTR filePath = L"C:\\jakewashere";

//current direcoty of vlc installer
std::wstring outFile = std::wstring(filePath) + L"\\vlc_installer.exe";

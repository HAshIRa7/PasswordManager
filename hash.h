#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <sstream>
#include "cryptopp/cryptlib.h"
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"
#include "cryptopp/files.h"
//#include "cionout.h"

std::string GetHash(const std::string& password, const std::string& salt);
std::string GenerateSalt(size_t N);
std::tuple<bool, std::string, std::string> GetHashWithSalt(const std::string& password);

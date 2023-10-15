#include "NetworkingUtils.h"

namespace ValveNetworking
{
	namespace Utils
	{
		/////////////////////////////////////////////////////////
		// Helper Functions
		/////////////////////////////////////////////////////////

		// trim from start (in place)
		void ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
				return !std::isspace(ch);
				}));
		}

		// trim from end (in place)
		void rtrim(std::string& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), s.end());
		}
	}
}
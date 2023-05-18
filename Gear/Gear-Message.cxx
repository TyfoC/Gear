#include "Gear-Message.hxx"

Gear::Message::operator std::string() const {
	std::string result = "";
	if (Type == MESSAGE_TYPE_ERROR) result = "Error: ";
	else if (Type == MESSAGE_TYPE_WARNING) result = "Warning: ";
	else result = "Information: ";
	result += Value;
	return result;
}
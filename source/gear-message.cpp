#include <gear-message.hpp>

Gear::Message_t::operator std::string() const {
	std::string prefix = "";
	if (Type == MessageType::ERROR) prefix = "Error: ";
	else if (Type == MessageType::WARNING) prefix = "Warning: ";
	else prefix = "Info: ";
	return prefix + Text + " (line: " + std::to_string(Line) + ", column: " + std::to_string(Column) + ')';
}

void Gear::PrintMessages(const std::vector<Message_t> messages, const std::string filePath, const std::string fileData) {
	if (!messages.size()) return;

	std::cout << "In file `" << filePath << "`:" << std::endl;
	for (const Message_t &message : messages) {
		std::cout << "\t" << (std::string)message;
		if (message.Line != std::string::npos) {
			std::cout << ":\t`" << std::string_view(fileData).substr(
				message.Position, Gear::GetLengthBeforeLineBreak(fileData, message.Position)
			) << '`';
		}
		std::cout << std::endl;
	}
}
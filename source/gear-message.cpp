#include <gear-message.hpp>

Gear::Message_t::operator std::string() const {
	std::string prefix = "";
	if (Type == MessageType::ERROR) prefix = "Error: ";
	else if (Type == MessageType::WARNING) prefix = "Warning: ";
	else prefix = "Info: ";
	return prefix + Text;
}

void Gear::PrintMessages(
	const std::vector<Message_t> messages,
	const std::string filePath,
	const std::string fileData,
	size_t realStartPosition
) {
	if (!messages.size()) return;

	std::cout << "In file `" << filePath << "`:" << std::endl;

	StringPosition_t strPos;
	for (const Message_t &message : messages) {
		std::cout << "\t" << (std::string)message;
		if (message.Position != std::string::npos) {
			strPos = GetLineColumnByPosition(fileData, realStartPosition + message.Position);
			std::cout << "(line: " << strPos.Line << ", column: " << strPos.Column << "):\t`" << std::string_view(fileData).substr(
				realStartPosition + message.Position,
				Gear::GetLengthBeforeLineBreak(fileData, realStartPosition + message.Position)
			) << '`';
		}
		std::cout << std::endl;
	}
}
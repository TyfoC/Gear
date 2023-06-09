#include <gear-message.hpp>

Gear::Message::operator std::string() const {
	std::string prefix = "";
	if (Type == Message::TYPE_ERROR) prefix = "Error: ";
	else if (Type == Message::TYPE_WARNING) prefix = "Warning: ";
	else prefix = "Info: ";
	return prefix + Text;
}

void Gear::PrintMessages(
	const std::vector<Message> messages,
	const std::string filePath,
	const std::string fileData,
	size_t realStartPosition
) {
	if (!messages.size()) return;

	std::cout << "In file `" << filePath << "`:" << std::endl;

	StringPosition strPos;
	for (const Message &message : messages) {
		std::cout << "\t" << (std::string)message;
		if (message.Position != std::string::npos) {
			strPos = GetStringPosition(fileData, realStartPosition + message.Position);
			std::cout << "(line: " << strPos.Line << ", column: " << strPos.Column << "):\t`" << std::string_view(fileData).substr(
				realStartPosition + message.Position,
				GetCharactersCountBeforeLineBreak(fileData, realStartPosition + message.Position)
			) << '`';
		}
		std::cout << std::endl;
	}
}
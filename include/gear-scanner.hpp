#pragma once

#include <iostream>
#include <regex>
#include "gear-token-templates.hpp"

namespace Gear {
	class Scanner {
		public:
		Scanner(const std::string &source);
		Token Scan();
		Token ScanNext();
		std::vector<Token> ScanToEnd();
		bool ScanExpression(std::vector<Token> &result);
		bool ScanExpressionNext(std::vector<Token> &result);
		bool ScanSeparatedExpressions(size_t separatorType, std::vector<std::vector<Token> > &result, bool skipRedundant = false);
		bool ScanSeparatedExpressionsNext(size_t separatorType, std::vector<std::vector<Token> > &result, bool skipRedundant = false);
		size_t GetPosition() const;
		size_t GetSourceLength() const;
		void SetPosition(size_t position);
		bool IsAtEnd() const;

		static bool ScanSeparatedExpressions(
			const std::vector<Token> &tokens,
			size_t index,
			size_t separatorType,
			std::vector<std::vector<Token> > &result,
			bool skipRedundant = false
		);
		private:
		std::string	Source_;
		size_t		Position_;
		size_t		SourceLength_;
	};
}
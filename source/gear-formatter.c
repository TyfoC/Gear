#include <gear-formatter.h>

char* gformat_value(char value) {
	switch (value) {
		case '\t':
			return gcopy_string("\\t");
		case '\v':
			return gcopy_string("\\v");
		case '\f':
			return gcopy_string("\\f");
		case '\b':
			return gcopy_string("\\b");
		case '\r':
			return gcopy_string("\\r");
		case '\n':
			return gcopy_string("\\n");
		case '\'':
			return gcopy_string("\\'");
		case '\"':
			return gcopy_string("\\\"");
		case '\\':
			return gcopy_string("\\\\");
	}

	char result[2] = { value, 0 };
	return gcopy_string(result);
}

char* gformat_lexemes(const char* source, const glexeme_t* lexemes, size_t count) {
	char* result = (char*)malloc(0);
	char* tmp;
	char* subTmp;
	for (size_t i = 0; i < count; i++) {
		tmp = gappend_string(result, "value = \"");
		if (!tmp) {
			if (result) free(result);
			return NULL;
		}

		result = tmp;

		for (size_t j = 0; j < lexemes[i].length; j++) {
			tmp = gformat_value(source[lexemes[i].position + j]);
			if (!tmp) {
				if (result) free(result);
				return NULL;
			}

			subTmp = gappend_string(result, tmp);
			if (!subTmp) {
				free(tmp);
				if (result) free(result);
				return NULL;
			}

			free(tmp);
			result = subTmp;
		}

		//	pos
		tmp = gappend_string(result, "\", pos = ");
		if (!tmp) {
			if (result) free(result);
			return NULL;
		}

		result = tmp;

		subTmp = gint_to_string(lexemes[i].position, 10);
		if (!subTmp) {
			if (result) free(result);
			return NULL;
		}

		tmp = gappend_string(result, subTmp);
		if (!tmp) {
			free(subTmp);
			if (result) free(result);
			return NULL;
		}

		free(subTmp);
		result = tmp;

		//	len
		tmp = gappend_string(result, ", len = ");
		if (!tmp) {
			if (result) free(result);
			return NULL;
		}

		result = tmp;

		subTmp = gint_to_string(lexemes[i].length, 10);
		if (!subTmp) {
			if (result) free(result);
			return NULL;
		}

		tmp = gappend_string(result, subTmp);
		if (!tmp) {
			free(subTmp);
			if (result) free(result);
			return NULL;
		}

		free(subTmp);
		result = tmp;

		//	group
		tmp = gappend_string(result, ", group = ");
		if (!tmp) {
			if (result) free(result);
			return NULL;
		}

		result = tmp;

		subTmp = gint_to_string(lexemes[i].group, 10);
		if (!subTmp) {
			if (result) free(result);
			return NULL;
		}

		tmp = gappend_string(result, subTmp);
		if (!tmp) {
			free(subTmp);
			if (result) free(result);
			return NULL;
		}

		free(subTmp);
		result = tmp;

		//	type
		tmp = gappend_string(result, ", type = ");
		if (!tmp) {
			if (result) free(result);
			return NULL;
		}

		result = tmp;

		subTmp = gint_to_string(lexemes[i].type, 10);
		if (!subTmp) {
			if (result) free(result);
			return NULL;
		}

		tmp = gappend_string(result, subTmp);
		if (!tmp) {
			free(subTmp);
			if (result) free(result);
			return NULL;
		}

		free(subTmp);
		result = tmp;

		tmp = gappend_string(result, "\n");
		if (!tmp) {
			if (result) free(result);
			return NULL;
		}

		result = tmp;
	}

	return result;
}
#include "src/HexEditor.hpp"

auto main() -> int {
	constexpr const char* version = "0.1";
	const std::string editorTitle = "x32 Hex Editor v" + std::string(version);

	// Console setup
	const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	const HWND console = GetConsoleWindow();

	COORD buffer = { 108, 9999 };
	SetConsoleScreenBufferSize(hOut, buffer);

	RECT consoleSz;
	GetWindowRect(console, &consoleSz);

	RECT monitorSz;
	GetWindowRect(GetDesktopWindow(), &monitorSz);

	// Center our Hex Editor	
	SetWindowPos(console, NULL, (monitorSz.right / 2) - ((consoleSz.right - consoleSz.left) / 2), (monitorSz.bottom / 2) - ((consoleSz.bottom - consoleSz.top) / 2), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Setup
	HexEditor* hexEditor = new HexEditor();
	namespace fs = std::filesystem;
	const char* currentPage = "";

	// Some lambdas which print out stuff
	auto printHelp = [&]() noexcept -> void {
		if (!strcmp(currentPage, "help"))
			return;

		currentPage = "help";
		system("cls");
		functions::setTitle(editorTitle + " | Help");

		console::colorformat("{color:light_red=x32 Hex Editor v%v}\n", version);
		console::colorformat("(c) 2020 AlienTheBetrayer. All rights reserved.\n\n");
		console::colorformat("All available hotkeys:\n");
		console::colorformat("[{color:light_blue=SHIFT + F}] - Select a new file and load it.\n");
		console::colorformat("[{color:light_blue=SHIFT + P}] - Print out all loaded file(might not fit in the console).\n");
		console::colorformat("[{color:light_blue=SHIFT + /}] - Shows this message.\n");
		console::colorformat("[{color:light_blue=SHIFT + D}] - Dumps the loaded file. All your dumps go to 'dumps' folder.\n");
	};

	printHelp();

	for (;;) {
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

		if (functions::isPressed(VK_LSHIFT)) {
			// New file : SHIFT + F
			if (functions::isPressed('f')) {
				const std::string selectedPath = functions::selectFile();
				if (selectedPath == "")
					continue;

				currentPage = "file edit";
				functions::setTitle(editorTitle + " | Loading " + hexEditor->getFilename() + "...");

				const functions::timepoint_t first = functions::timepoint();
				hexEditor->openAndSetup(selectedPath.c_str());
				hexEditor->read();
				const functions::timepoint_t last = functions::timepoint();

				const double ms = functions::getDifference(first, last);
				functions::setTitle(editorTitle + " | Loaded " + hexEditor->getFilename() + " | " + std::to_string(hexEditor->length()) + " bytes | took " + std::to_string(ms) + " s.");
			}
			// Help : SHIFT + /
			else if (functions::isPressed('/')) {
				printHelp();
			}
			// Print : SHIFT + P
			else if (functions::isPressed('p')) {
				system("cls");
				currentPage = "print";

				functions::setTitle(editorTitle + " | Printing " + hexEditor->getFilename() + "... | " + std::to_string(hexEditor->length()) + " bytes");
				
				const functions::timepoint_t first = functions::timepoint();
				hexEditor->print();
				const functions::timepoint_t last = functions::timepoint();

				const double ms = functions::getDifference(first, last);
				functions::setTitle(editorTitle + " | Printed " + hexEditor->getFilename() + "... | " + std::to_string(hexEditor->length()) + " bytes | took " + std::to_string(ms) + " s.");
			}
			// Dump : SHIFT + D
			else if (functions::isPressed('d')) {
				currentPage = "dumping";

				functions::setTitle(editorTitle + " | Dumping " + hexEditor->getFilename() + "... | " + std::to_string(hexEditor->length()) + " bytes");
				
				const functions::timepoint_t first = functions::timepoint();
				hexEditor->print(true);
				const functions::timepoint_t last = functions::timepoint();

				const double ms = functions::getDifference(first, last);
				functions::setTitle(editorTitle + " | Dumped " + hexEditor->getFilename() + " | " + std::to_string(hexEditor->length()) + " bytes | took " + std::to_string(ms) + " s.");
			}
		}
	}

	delete hexEditor;
}
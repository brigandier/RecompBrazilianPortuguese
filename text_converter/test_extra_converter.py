import tempfile
import unittest
from pathlib import Path

import extra_converter as converter


class ExtraTextTests(unittest.TestCase):
    def setUp(self):
        self.messages = converter.read_messages(converter.ROOT / "text_files/[43] - Extra Text.txt")
        self.widths = converter.font_widths(converter.ROOT)

    def test_current_inputs(self):
        converter.validate_messages(converter.ROOT, self.messages)
        cues = converter.read_cues(converter.ROOT / "text_files/[44] - Intro Subtitles.txt", self.widths)
        self.assertEqual(len(cues), 13)

    def test_fixed_slot_and_format_limits(self):
        for key, text in [("JETPAC_EXIT", "A" * 16), ("ARCADE_TIME", "TEMPO %s")]:
            with self.subTest(key=key), self.assertRaises(ValueError):
                converter.validate_messages(converter.ROOT, self.messages | {key: text})

    def test_duplicate_and_missing_keys(self):
        original = (converter.ROOT / "text_files/[43] - Extra Text.txt").read_text(encoding="utf-8")
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "extra_texts.txt"
            for text in [original + "GAME_OVER = OUTRO TEXTO\n", "GAME_OVER = FIM DE JOGO\n"]:
                path.write_text(text, encoding="utf-8")
                with self.assertRaises(ValueError):
                    converter.read_messages(path)

    def test_invalid_subtitles(self):
        examples = ["1|2|UNKNOWN|TESTE|", "2|1|KROOL|TESTE|",
                    "1|3|KROOL|TESTE|\n2|4|KLUMP|TESTE|",
                    "nan|3|KROOL|TESTE|", "1|2|KROOL|🙂|",
                    "1|2|KROOL|" + "M" * 100 + "|"]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "intro_subtitles.txt"
            for text in examples:
                path.write_text(text, encoding="utf-8")
                with self.subTest(text=text), self.assertRaises(ValueError):
                    converter.read_cues(path, self.widths)

    def test_edit_and_encoding(self):
        self.assertEqual(converter.c_string('AÇÃO "\\'), '"A\\307\\303O \\042\\134"')
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "intro_subtitles.txt"
            path.write_text("1.25|3.5|KLUMP|OLÁ!|TUDO BEM?\n", encoding="utf-8")
            self.assertEqual(converter.read_cues(path, self.widths),
                             [(1.25, 3.5, "KLUMP", "OLÁ!", "TUDO BEM?")])


if __name__ == "__main__":
    unittest.main()

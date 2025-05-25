# Building the whitepaper

Even though the whitepaper is written in markdown, it's not supported by markdown. It uses *pandoc*. You can generate it by running:
```bash
pandoc whitepaper_esp.md -o whitepaper.pdf --pdf-engine=xelatex --include-in-header=header.tex --number-sections
```
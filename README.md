
<h1>
<img src="SVM_PE_WALL.jpg" alt="SVM PE" align="left">
SVM PE Pixel Extraction Utility Program
</h1>
<p>
An easy-to-use software which can extract <b>pixel data</b> from <b>1-bpp Monochrome Bitmaps</b>
and transform those extracted data into <b>ESC\POS Single-Bit Rasters</b>.
</p>

<img src="SVM_PE_SCRN1.jpg" alt="Main Window" align="right" width="50%" height="50%">

<br>
<h3>
Printing Guidelines
</h3>
<p align="justify">
The single-bit raster image produced by SVM PE is generally <b>row-ordered</b>. That means, consecutive pixel data inside the produced image is arranged in rows.<br>So to print this raster image by Thermal POS printers,
the <b>most suitable ESC\POS command</b> would be:
<br><b>GS v 0 m xL xH yL yH d1...dk</b>
<br>where <b>k = (xL + xH * 256) + (yL + yH * 256)
</p>

<h3>Download</h3>
<ul>
<li>
<a href="https://github.com/AKD92/SVM-PE-Pixel-Extraction-Utility-Program/raw/master/bin/SVM_PE_UTIL.EXE">Stand-alone Binary Executable (Win32)
<br></a> Compiled using <a href="http://tdm-gcc.tdragon.net/about">TDM-GCC 5.1</a>
</li>
<li>
<a href="/src">Browse for Source Codes in C</a> on GitHub
</li>
</ul>

<h3>Related Links</h3>
<ol>
<li>
<a href="http://content.epson.de/fileadmin/content/files/RSD/downloads/escpos.pdf">ESC/POS at Epson</a>
</li>
<li>
<a href="https://en.wikipedia.org/wiki/Page_description_language">Page Description Language</a>
</li>
<li>
<a href="http://www.fileformat.info/format/bmp/egff.htm">Microsoft Windows BITMAP File Format Summary</a>
</li>
<li>
<a href="https://msdn.microsoft.com/en-us/library/windows/desktop/dd183376(v=vs.85).aspx">
Bitmap Header Structure @ MSDN Developer Resource</a>
</li>
</ol>

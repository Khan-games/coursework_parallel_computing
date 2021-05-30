<h2>Inverted index</h2>
Can be used to quickly search for text in large file directories.

<h3>Build</h3>
<h4>Requirements:</h4>
<ul>
<li>C++17</li>
<li>Boost v1.76.0 or higher</li>
<li>Visual Studio 2019</li>
</ul>
<h4>Steps:</h4>
<ol>
<li>Clone repository</li>
<li>Open solution in VS (coursework_parallel_computing.sln)</li>
<li>For every project in the solution set up Boost library: 
<ol>
<li>Right click and select Properties</li>
<li>Open Common Properties and select VC++ Directories</li>
<li>Add the directory where you installed boost to Include Directories</li>
<li>Add the directory where you built boost libraries to Library Directories</li>
</ol></li>
<li>Turn on C++17 support</li>
<li>(Un)comment prefered defines that turns on Logging\Testing systems (<i>#define</i> begins with <i>LOG_</i> or <i>TEST_</i> accordingly) </li>
<li>Compile solution</li>
</ol>

<h3>Using</h3>
<h4>To create index:</h4>
<ol>
<li>Place your directories into "index_creator/dataset" folder</li>
<li>Compile and run index_creator, created index would be stored into "index_creator/index.index" file</li>
</ol>


<h4>To use client/server bundle</h4> 
  Set up client's main() with creating Client objects and pass to them requests within vector of strings. 
You can also change default number of threads used by index_creator in its main().

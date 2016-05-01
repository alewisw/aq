
### Initial State

<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#ffff66" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#bfbfbf" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#ffff66" valign="middle" align="center">4
    <td bgcolor="#66ff66" valign="middle" align="center">5
    <td bgcolor="#66ff66" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#ffff66" valign="middle" align="center">10
    <td bgcolor="#bfbfbf" valign="middle" align="center">11
    <td bgcolor="#bfbfbf" valign="middle" align="center">12
    <td bgcolor="#bfbfbf" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve<br>release
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="3">3 pages<br>commit<br>released
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#ffff66" valign="top" align="center">DD
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#ffff66" valign="top" align="center">HH
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="3">AA AA AA
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snapshot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 1<br>released<br>
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 2<br>incomplete<br>released<br>
    <td bgcolor="#ffff66" valign="top" align="center">index 3
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">index 4<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 5
    <td bgcolor="#ff9933" valign="top" align="center">index 6<br>incomplete
    <td bgcolor="#ffff66" valign="top" align="center">index 7
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="3">Index 0<br>released
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td valign="top" align="center" colspan="3">3 pages
    <td colspan="8">
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="4">4 pages
    <td width="0%">
</tr>

</table>

### Claim Operation - AQWriter::claim()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#ffff66" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#8f8f8f" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#ffff66" valign="middle" align="center">4
    <td bgcolor="#66ff66" valign="middle" align="center">5
    <td bgcolor="#66ff66" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#ffff66" valign="middle" align="center">10
    <td bgcolor="#ff9933" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve<br>release
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#ffff66" valign="top" align="center">DD
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#ffff66" valign="top" align="center">HH
    <td bgcolor="#ff9933" valign="top" align="center">ii
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 1<br>incomplete<br>released
    <td bgcolor="#ffff66" valign="top" align="center">index 2
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">index 3<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 4
    <td bgcolor="#ff9933" valign="top" align="center">index 5<br>incomplete
    <td bgcolor="#ffff66" valign="top" align="center">index 6
    <td bgcolor="#8f8f8f" valign="top" align="center">
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="3">3 pages
    <td colspan="9">
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="3">3 pages
    <td width="0%">
</tr>
</table>


### Commit Operation - AQWriter::commit()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#ffff66" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#8f8f8f" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#ffff66" valign="middle" align="center">4
    <td bgcolor="#66ff66" valign="middle" align="center">5
    <td bgcolor="#66ff66" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#ffff66" valign="middle" align="center">10
    <td bgcolor="#ffff66" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve<br>release
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#ffff66" valign="top" align="center">DD
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#ffff66" valign="top" align="center">HH
    <td bgcolor="#ffff66" valign="top" align="center">II
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 1<br>incomplete<br>released
    <td bgcolor="#ffff66" valign="top" align="center">index 2
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">index 3<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 4
    <td bgcolor="#ff9933" valign="top" align="center">index 5<br>incomplete
    <td bgcolor="#ffff66" valign="top" align="center">index 6
    <td bgcolor="#ffff66" valign="top" align="center">index 7
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="3">3 pages
    <td colspan="9">
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="3">3 pages
    <td width="0%">
</tr>

</table>

### Retrieve Operation - AQReader::retrieve()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#99ffcc" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#8f8f8f" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#99ffcc" valign="middle" align="center">4
    <td bgcolor="#66ff66" valign="middle" align="center">5
    <td bgcolor="#66ff66" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#ffff66" valign="middle" align="center">10
    <td bgcolor="#ffff66" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve<br>release
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#99ffcc" valign="top" align="center">DD
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#ffff66" valign="top" align="center">HH
    <td bgcolor="#ffff66" valign="top" align="center">II
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 1<br>incomplete<br>released
    <td bgcolor="#99ffcc" valign="top" align="center">index 2
    <td bgcolor="#66ff66" valign="top" align="center" colspan="2">index 3<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 4
    <td bgcolor="#ff9933" valign="top" align="center">index 5<br>incomplete
    <td bgcolor="#ffff66" valign="top" align="center">index 6
    <td bgcolor="#ffff66" valign="top" align="center">index 7
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="3">3 pages
    <td colspan="9">
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="3">3 pages
    <td width="0%">
</tr>

</table>



### Release Operation - AQReader::release()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#99ffcc" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#8f8f8f" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#bfbfbf" valign="middle" align="center">4
    <td bgcolor="#bfbfbf" valign="middle" align="center">5
    <td bgcolor="#bfbfbf" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#ffff66" valign="middle" align="center">10
    <td bgcolor="#ffff66" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center">1 page<br>commit<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#bfbfbf" valign="top" align="center">DD
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#ffff66" valign="top" align="center">HH
    <td bgcolor="#ffff66" valign="top" align="center">II
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 1<br>incomplete<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center">index 2<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 3<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 4
    <td bgcolor="#ff9933" valign="top" align="center">index 5<br>incomplete
    <td bgcolor="#ffff66" valign="top" align="center">index 6
    <td bgcolor="#ffff66" valign="top" align="center">index 7
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="6">6 pages
    <td colspan="6">
    <td valign="top" align="center" colspan="3">3 pages
    <td width="0%">
</tr>

</table>



### Retrieve Operation Starts Expiry Timer - AQReader::retrieve()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#99ffcc" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#8f8f8f" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#bfbfbf" valign="middle" align="center">4
    <td bgcolor="#bfbfbf" valign="middle" align="center">5
    <td bgcolor="#bfbfbf" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#99ffcc" valign="middle" align="center">10
    <td bgcolor="#ffff66" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center">1 page<br>commit<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim<br>xtimer-on
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#ffff66" valign="top" align="center">1 page<br>claim<br>commit
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#bfbfbf" valign="top" align="center">DD
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#99ffcc" valign="top" align="center">HH
    <td bgcolor="#ffff66" valign="top" align="center">II
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 1<br>incomplete<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center">index 2<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 3<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 4
    <td bgcolor="#ff9933" valign="top" align="center">index 5<br>incomplete
    <td bgcolor="#99ffcc" valign="top" align="center">index 6
    <td bgcolor="#ffff66" valign="top" align="center">index 7
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="6">6 pages
    <td colspan="6">
    <td valign="top" align="center" colspan="3">3 pages
    <td width="0%">
</tr>

</table>

### Retrieve Operation When Timer Expires - AQReader::retrieve()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#99ffcc" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#8f8f8f" width="6%" align="center">Head
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#bfbfbf" valign="middle" align="center">0
    <td bgcolor="#bfbfbf" valign="middle" align="center">1
    <td bgcolor="#ff5050" valign="middle" align="center">2
    <td bgcolor="#ff5050" valign="middle" align="center">3
    <td bgcolor="#bfbfbf" valign="middle" align="center">4
    <td bgcolor="#bfbfbf" valign="middle" align="center">5
    <td bgcolor="#bfbfbf" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#99ffcc" valign="middle" align="center">10
    <td bgcolor="#99ffcc" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">2 pages<br>claim<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center">1 page<br>commit<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim<br>expired
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">unused
    <td bgcolor="#8f8f8f" valign="top" align="center">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">BB BB
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">cc cc
    <td bgcolor="#bfbfbf" valign="top" align="center">DD
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#99ffcc" valign="top" align="center">HH
    <td bgcolor="#99ffcc" valign="top" align="center">II
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f" valign="top" align="center">aa
    <td bgcolor="#8f8f8f">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#ff5050" valign="top" align="center" colspan="2">index 1<br>incomplete<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center">index 2<br>released
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 3<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 4
    <td bgcolor="#ff9933" valign="top" align="center">index 5<br>incomplete
    <td bgcolor="#99ffcc" valign="top" align="center">index 6
    <td bgcolor="#99ffcc" valign="top" align="center">index 7
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="6">6 pages
    <td colspan="6">
    <td valign="top" align="center" colspan="3">3 pages
    <td width="0%">
</tr>

</table>

### Claim Operation When Timer Expires - AQWriter::claim()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#bfbfbf" width="6%" align="center">Head
    <td width="6%">
    <td bgcolor="#99ffcc" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#ff9933" valign="middle" align="center">0
    <td bgcolor="#ff9933" valign="middle" align="center">1
    <td bgcolor="#ff9933" valign="middle" align="center">2
    <td bgcolor="#ff9933" valign="middle" align="center">3
    <td bgcolor="#ff9933" valign="middle" align="center">4
    <td bgcolor="#bfbfbf" valign="middle" align="center">5
    <td bgcolor="#bfbfbf" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff9933" valign="middle" align="center">9
    <td bgcolor="#99ffcc" valign="middle" align="center">10
    <td bgcolor="#99ffcc" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#ff9933" valign="top" align="center" colspan="5">5 pages<br>claim
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff9933" valign="top" align="center">1 page<br>claim<br>expired
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#8f8f8f" valign="top" align="center" colspan="3">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#ff9933" valign="top" align="center" colspan="5">jj jj jj jj jj
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff9933" valign="top" align="center">gg
    <td bgcolor="#99ffcc" valign="top" align="center">HH
    <td bgcolor="#99ffcc" valign="top" align="center">II
    <td bgcolor="#8f8f8f" colspan="3">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#8f8f8f" valign="top" align="center" colspan="5">
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 1
    <td bgcolor="#ff9933" valign="top" align="center">index 2<br>incomplete
    <td bgcolor="#99ffcc" valign="top" align="center">index 3
    <td bgcolor="#99ffcc" valign="top" align="center">index 4
    <td bgcolor="#8f8f8f" valign="top" align="center" colspan="3">
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td colspan="5">
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="1">1 page
    <td colspan="9">
    <td width="0%">
</tr>

</table>

### Retrieve Operation When Timer Expires and Queue > 75% Full - AQWriter::retrieve()
<table>
<tr>
    <td border="0" width="10%">Pointers:
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td bgcolor="#bfbfbf" width="6%" align="center">Head
    <td width="6%">
    <td bgcolor="#99ffcc" width="6%" align="center">Tail
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
    <td width="6%">
</tr>

<tr>
    <td border="0">Index:</td>
    <td bgcolor="#ff9933" valign="middle" align="center">0
    <td bgcolor="#ff9933" valign="middle" align="center">1
    <td bgcolor="#ff9933" valign="middle" align="center">2
    <td bgcolor="#ff9933" valign="middle" align="center">3
    <td bgcolor="#ff9933" valign="middle" align="center">4
    <td bgcolor="#bfbfbf" valign="middle" align="center">5
    <td bgcolor="#bfbfbf" valign="middle" align="center">6
    <td bgcolor="#99ffcc" valign="middle" align="center">7
    <td bgcolor="#99ffcc" valign="middle" align="center">8
    <td bgcolor="#ff5050" valign="middle" align="center">9
    <td bgcolor="#99ffcc" valign="middle" align="center">10
    <td bgcolor="#99ffcc" valign="middle" align="center">11
    <td bgcolor="#8f8f8f" valign="middle" align="center">12
    <td bgcolor="#8f8f8f" valign="middle" align="center">13
    <td bgcolor="#8f8f8f" valign="middle" align="center">14
    <td width="0%">
</tr>

<tr>
    <td border="0">Control:</td>
    <td bgcolor="#ff9933" valign="top" align="center" colspan="5">5 pages<br>claim
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">2 pages<br>commit<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">2 pages<br>claim<br>commit<br>retrieve
    <td bgcolor="#ff5050" valign="top" align="center">1 page<br>claim<br>retrieve
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#99ffcc" valign="top" align="center">1 page<br>claim<br>commit<br>retrieve
    <td bgcolor="#8f8f8f" valign="top" align="center" colspan="3">waste
    <td width="0%">
</tr>

<tr>
    <td border="0">User Data:</td>
    <td bgcolor="#ff9933" valign="top" align="center" colspan="5">jj jj jj jj jj
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">EE EE
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">FF FF
    <td bgcolor="#ff5050" valign="top" align="center">gg
    <td bgcolor="#99ffcc" valign="top" align="center">HH
    <td bgcolor="#99ffcc" valign="top" align="center">II
    <td bgcolor="#8f8f8f" colspan="3">
    <td width="0%">
</tr>

<tr>
    <td border="0">Snaphot:</td>
    <td bgcolor="#ff9933" valign="top" align="center" colspan="5">index 5<br>incomplete
    <td bgcolor="#bfbfbf" valign="top" align="center" colspan="2">index 0<br>released
    <td bgcolor="#99ffcc" valign="top" align="center" colspan="2">index 1
    <td bgcolor="#ff5050" valign="top" align="center">index 2<br>incomplete
    <td bgcolor="#99ffcc" valign="top" align="center">index 3
    <td bgcolor="#99ffcc" valign="top" align="center">index 4
    <td>
    <td>
    <td>
    <td width="0%">
</tr>

<tr>
    <td border="0">Available Size:</td>
    <td colspan="5">
    <td bgcolor="#cc66ff" valign="top" align="center" colspan="1">1 page
    <td colspan="9">
    <td width="0%">
</tr>

</table>

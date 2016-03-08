**View from [StackEdit](https://stackedit.io/viewer#!url=https://github.com/gpascualg/ReverseTale/raw/master/server/Parsing/Parsing.md) for a correct format**

------------------

Nostale packets
============

We will denote **\>\>** as a sent packet and **<<** as a received packet.

Login
-------
\>\> **NoS0575**
> NoS0575
> [6-8] Random
> Username
> Hashed Password
> [8] Random
> [1] 0x0B
> Version (0.9.3.3055) 0
> [32] MD5
> [1] 0x0A

<< **Login Result**: OK
> NsTeST
> [1-8] SessionID
> *For each GameServer*
> > ip:port:Capacity:Server.Channel.Name
> 
> -1.-1.-1.-1:10000.10000.4

<< **Login Result**: Failed
> failed
> Error message

GameServer Initialize
------------------------

\>\> **Connect sequence**
> [6-8] Alive Counter
> [1-8] Session ID

\>\> **Login Information** (two in one)
> > Alive++
> > Username
> 
> 0xFF
> > Alive++
> > Password (plain)

<< **Character Selection**
> clist_start
> 0

<< **Character List**
> *For each Character*
> > clist
> > Slot (0-2)
> > Name
> > 0
> > Sex (0=male/1=female)
> > Hair Style (0=A/1=B)
> > Color (0-9?)
> > 0xFF
>
> clist_end

\>\> **End receiving list** (1)
> Alive++
> c_close

\>\> **End receiving list** (2)
> Alive++
> f_stash_end


Character Manipulating
---------------------------

\>\> **Create Character**
> Alive++
> Char_NEW
> name
> Slot
> Sex
> Hair
> Color

Response is sent as a *c_list* packet (characters listing)

\>\> **Delete character**
> Alive++
> Char_DEL
> Slot
> Password (plain)

Response is sent as a *c_list* packet (characters listing)


Alive
------
Periodically sent every 5 seconds, both in character selection and ingame
> Alive++
> 0


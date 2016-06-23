http://www.remkoweijnen.nl/blog/2008/03/02/how-rdp-passwords-are-encrypted-2/

decrypting RDP passwords. I left one thing open: encrypting the password up to the full 1329 bytes as mstsc does.

Many people were curious about it so I hope the answer is not a disappointment because it’s actually really simple (but I took me a while to figure that out nonetheless). In what I figure is an attempt to hide the password length mstsc always fills up the password with zeroes until it has 512 bytes length.

Then the password is encrypted like I described earlier which gives us a 1328 bytes password hash. So we have one mystery left, how to reach the 1329 bytes size which still is a strange value since the password is in Unicode which takes 2 bytes per char (so the size should be even).

As it turns out, mstsc just adds a zero!


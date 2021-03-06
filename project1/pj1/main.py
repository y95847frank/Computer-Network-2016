from __future__ import division
import socket
import urllib2
import httplib
from bs4 import BeautifulSoup
import sys
import random
from collections import OrderedDict

operations = OrderedDict([
    ("+", lambda x, y: x + y),
    ("-", lambda x, y: x - y),
    ("/", lambda x, y: x / y),
    ("*", lambda x, y: x * y),
    ("^", lambda x, y: x ** y)
])
op = operations.keys()

def simplify(string) :
    t = []
    while string :
        first = string[0:1]
        string = string[1:len(string)]
        if first == "(" :
            try:
                paren, string = simplify(string)
                t.append(paren)
            except:
                return "opps"
        elif first == ")" :
            return t, string
        elif first.isdigit() or first == "." :
            try :
                if t[-1] in op:
                    t.append(first)
                elif type(t[-1]) is list:
                    return "opps"
                else :
                    t[-1] += first
            except :
                t.append(first)
        elif first in op :
            t.append(first)
        elif first.isspace() :
            pass
        else:
            return "opps"
    return t

def evaluate(t):
    print "!!!"
    print t
    for op, func in operations.items():
        try:
            pos = t.index(op)
            left = evaluate(t[:pos])
            right = evaluate(t[pos+1:])
            return func(left, right)
        except:
            pass
    if len(t) == 1:
        try :
            return float(t[0])
        except:
            return evaluate(t[0])
    else :
        return "opps"


def calc(expr):
    c = expr
    c = c.replace("(", "")
    c = c.replace(")", "")
    if c.find("**") != -1 or c.find("++") != -1 or c.find("+-") != -1 or c.find("--") != -1 or c.find("-+") != -1:
        return "opps"
    elif c.find("//") != -1 or c.find("*-") != -1 or c.find("*+") != -1 or c.find("/+") != -1 or c.find("/-") != -1:
        return "opps"
    elif c.find("/*") != -1 or c.find("*/") != -1:
        return "opps"
    #expr = expr.replace("^", "**")
    check = expr
    check = check.replace("+", "")
    check = check.replace(" ", "")
    check = check.replace("-", "")
    check = check.replace("*", "")
    check = check.replace("/", "")
    check = check.replace("(", "")
    check = check.replace(")", "")
    check = check.replace(".", "")
    check = check.replace("^", "")
    if check.isdigit() != True :
        return "opps"
    else :
        try :
            #return float(eval(expr))
            return evaluate(simplify(expr))
        except :
            return "ahh"

channel = ""
password = ""
with open("config") as f:
    content = f.readlines()
#print content
channel = content[0][6:len(channel)-2]
password = content[1][10:len(password)-2]
#print channel, password

irc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
irc.connect(("irc.freenode.net", 6667))
print irc.recv(4096)

irc.send('PASS toughs\r\n')
print irc.recv(4096)
nick = "FrankYen"
n_msg = "NICK " + nick + "\r\n"
irc.send(n_msg)
print irc.recv(4096)
irc.send('USER x x x x\r\n')
print irc.recv(4096)
join_msg =  "JOIN "+ channel+" "+ password+"\r\n"
irc.send(join_msg)
print join_msg
print irc.recv(4096)


hello_msg = "PRIVMSG "+ channel+ " :Hola. Buenos tardes.\r\n"
irc.send(hello_msg)
play_on = ""
count = 5
ans = 77
while(1) :
	buf = irc.recv(4096)
	print buf
        msg = "PRIVMSG "
        msg += channel
        msg += " :"
	if buf.find("PING") != -1:
            t = buf.find("PING")
            if(t < 3) :
		buf.replace("PING", "PONG", 1)
		irc.send(buf)
                continue
        check_buf = "PRIVMSG "+channel+" :"
        c_buf = buf.find(check_buf)
        #print check_buf, buf[c_buf+10+len(channel):c_buf+17+len(channel)]
        if buf[c_buf+10+len(channel):c_buf+17+len(channel)] == "@repeat":    
	    t = buf.find("@repeat")
	    t += 8
	    name = buf.find("!")
	    msg += buf[t:len(buf)-2]
            msg += " ("
	    msg = msg +buf[1:name]
	    msg = msg + ")\r\n"
	    print msg
	    irc.send(msg)
	elif buf[c_buf+10+len(channel):c_buf+15+len(channel)] == "@help":
	    n = buf.find("!")
            name = buf[1:n]
            msg = "PRIVMSG "+channel+" :@repeat <String> ("+name+")\r\n" 
            msg += "PRIVMSG "+channel+" :@cal <Expression> ("+name+")\r\n"
            msg += "PRIVMSG "+channel+" :@play <Robot Name> ("+name+")\r\n"
            msg += "PRIVMSG "+channel+" :@guess <Integer> ("+name+")\r\n"
            msg += "PRIVMSG "+channel+" :@google <String> ("+name+")\r\n"
	    irc.send(msg)
        elif buf[c_buf+10+len(channel):c_buf+15+len(channel)] == "@play":
            vertify = "@play " + nick
            if len(play_on) != 0:
                continue
            elif buf.find(vertify) == -1:
                continue
	    n = buf.find("!")
            name = buf[1:n]
            play_on = name
            ans = random.randint(0, 101)
            print "****** "+str(ans)+" ******"
            msg = "PRIVMSG "+channel+" :Start! (0-100 with 5 times) ("+name+")\r\n" 
            irc.send(msg)
        elif buf[c_buf+10+len(channel):c_buf+16+len(channel)] == "@guess":
	    n = buf.find("!")
            name = buf[1:n]
            if name != play_on :
                continue
            t = buf.find("@guess")
            t += 7
            num = buf[t:len(buf)-2]
            print num
            count -= 1
            if num.isdigit() == False :
                if count != 0 :
                    msg = "PRIVMSG "+channel+" :Error!!! - Check your input! ("+str(count)+") ("+name+")\r\n" 
                else :
                    msg = "PRIVMSG "+channel+" :Game Over! (ANS "+str(ans)+") ("+str(count)+") ("+name+")\r\n" 
                irc.send(msg)
            else :
                num = int(num)
                if num < ans :
                    if count != 0 :
                        msg = "PRIVMSG "+channel+" :Higher ("+str(count)+") ("+name+")\r\n" 
                    else :
                        msg = "PRIVMSG "+channel+" :Game Over! (ANS "+str(ans)+") ("+str(count)+") ("+name+")\r\n" 
                elif num > ans :
                    if count != 0 :
                        msg = "PRIVMSG "+channel+" :Lower ("+str(count)+") ("+name+")\r\n" 
                    else :
                        msg = "PRIVMSG "+channel+" :Game Over! (ANS "+str(ans)+") ("+str(count)+") ("+name+")\r\n" 
                elif num == ans:
                    msg = "PRIVMSG "+channel+" :Correct! ("+str(count)+") ("+name+")\r\n"
                    count = 5
                    play_on = ""
                irc.send(msg)
            if count == 0:
                count = 5
                play_on = ""
        elif buf[c_buf+10+len(channel):c_buf+14+len(channel)] == "@cal":
	    n = buf.find("!")
            name = buf[1:n]
            t = buf.find("@cal")
            t += 5
            line = buf[t:len(buf)-2]
            print line
            a = calc(line)
            print a
            if a == "opps":
                msg = "PRIVMSG "+channel+" :Error - Check your Input! ("+name+")\r\n" 
                irc.send(msg)
            elif a == "ahh" :
                msg = "PRIVMSG "+channel+" :Error - Check your Input! ("+name+")\r\n" 
                irc.send(msg)
            else :
                msg = "PRIVMSG "+channel+" :"+str(a)+" ("+name+")\r\n" 
                irc.send(msg)
        elif buf[c_buf+10+len(channel):c_buf+17+len(channel)] == "@google":
	    n = buf.find("!")
            name = buf[1:n]
            t = buf.find("@google")
            t += 8
            query = buf[t:len(buf)-2]
            query = query.replace(" ", "+")
            print query

            url = "https://www.google.com.tw/search?q="+query
            print url
            opener = urllib2.build_opener()
            opener.addheaders = [('User-agent', 'Mozilla/5.0')]
            try :
                p = opener.open(url)
            except :
                continue
            soup = BeautifulSoup(p)
            for s in soup.findAll('cite') :
                msg = "PRIVMSG "+channel+" :"+s.text+" ("+name+")\r\n" 
                irc.send(msg)
                print s.text
                break
            if len(soup.findAll('cite')) == 0:
                msg = "PRIVMSG "+channel+" :Error- didn't find anything! ("+name+")\r\n" 
                irc.send(msg)
                


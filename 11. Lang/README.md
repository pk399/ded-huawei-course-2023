# ПРПГ
Плаiн Рussuан Пгоgгаммiнg Ланguаgе
(subject to change)))))

Before parsing the file, all whitespace is removed

File = Func*
Id = {А-Я}{а-я | А-Я | - | _ | 0-9}*
Func = 'функ' Id AnyOP
AnyOP = {'-' OP '.' | PercentedOPs}
PercentedOPs = '%' OP* '%'
OP = Id '=' Exp '.' | 'пока' LExp AnyOP | 'если' LExp 'то' AnyOP {'иначе' AnyOP}? | 'скип.' | 'привет' Id '.'
LExp = ALExp {'или' ALExp}*
ALExp = NLExp {'и' NLExp}*
NLExp = {'не'}* RLExp
RLExp = Exp {'боль' | 'мень' | 'равн'} Exp


// TODO CSVA comma separated variable assignements
//      break

все буквы - lowercase
WP whitespace - обычно игнорируется
level = 0
File = Func*
Id = {а-я}{а-я | 0-9}*
IId = Id // Imprecise Id
Bye = 'оке' | 'байбай' | 'это' Exp | 'всё' // <- break
Func = 'нач' Id Hashtag* OP* Bye
VarDecl = Id Calls*
Hashtag = '#' VarDecl
IF = {'а'}? 'чо ели' Exp {'один' | 'ноль' | 'сущ' | '>0' | '<0'} '?'
IFS = OP+ '!' // OP* '!'
Calls = '[' Exp ']'
OP = IId '=' Exp | IF IFS {'а чо?' IFS}? | 'топ' {Exp | 'хз'} {Id | '_'} ':' !level++! {{Num '.'}xlevel OP}* !level--! | '$' Exp | '#' VarDecl {'=' Exp}? | 'assert(' SExp ')' | Bye
Exp = {'$'}? {IId {Calls*}? | Num | 'М(' SExp ')'}
SExp = MExp {{'+' | '-'} MExp}*
MExp = EExp {{'*' | '/'} EExp}*
EExp = {'-'}? PExp {'^' PExp}?
PExp = Num | IId {Calls*}? | '(' Exp ')'
Num = {1-9}{0-9}*

**!!! Variable names can't start with 'чоели', 'ачоели', 'топ'**

~~Замечание: пока что if - только 'чо ели' ... ?~~

КАК ПОСЧИТАТЬ ФАКТОРИАЛ????????

нач факториал #цифра
    чо ели цифра один?
        это один!
    это М(ц * [факт][ц - 1])

начинаем факториал а
ЧО ели а одинок?
дроп один!
А пачиму?
дроп М(а * [факт][а-1])!
окэй

начинаем черноголовку(
Берём циферку
Шоу М([факт][циферка])
байбай

байбай

начинаем факториал
Чо ели М(АбОнГа + КаМс) крутой?
Берём сперму, щитпостим М()

Топ хз амогусов:
1. 


[факт]

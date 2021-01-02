window.onload = function(){
	canvSize = 400;
	ranks = files = 8;
	squareSize = canvSize / ranks;
	mouseCol = -1;
	mouseRow = -1;
	selSquare = -1;
	specialSquares = [];
	blackView = false;

	pieces = [];
	side = "both";
	castlePermission = "KQkq";
	enPas = "-";
	fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	fen = "1r1qkb1r/1bp2pp1/p2p1n1p/3Np3/2pPP3/5N2/PPPQ1PPP/R1B2RK1 w k - 0 13";

	configureCanvas();
	setInterval(drawCanvas, 1000/5);

}

function configureCanvas(){
	let canvas = document.getElementById("canv");
	canvas.width = canvSize;
	canvas.height = canvSize;
	//canvas.addEventListener("mousemove", (event) => {trackmouse(event);});
	canvas.addEventListener("mousedown", (event) => {trackmouse(event);});
}

function switchView(){
	blackView = !blackView;
}

function updateSide(){
	document.getElementsByClassName("tomove")[0].innerHTML = side;
}

function switchSide(){
	if(side == "white"){ side = "black"; }
	else if(side == "black"){ side = "white"; }
	updateSide();
}

function drawCanvas(){
	let canv = document.getElementById("canv");
	let ctx = canv.getContext("2d");
	let light = {
		r: 255,
		g: 224,
		b: 102
	};
	let dark = {
		r: 128,
		g: 102,
		b: 0
	};
	let special = {
		r: 245,
		g: 66,
		b: 93
	};

	for(let y = 0 ; y < files ; y++){
		for(let x = 0 ; x < ranks ; x++){
			let color = {};
			Object.assign(color, (((x + y) % 2 == 0) ? light : dark));
			if(rc2Index(y,x) == selSquare){
				color.r *= 1.8 ; color.g *= 1.8 ; color.b *= 1.8 ;
			} else if(specialSquares.includes(rc2Index(y,x))){
				Object.assign(color, special);
			}

			ctx.fillStyle = "rgb("+color.r+", "+color.g+", "+color.b+")";
			ctx.fillRect(x * squareSize, y * squareSize, squareSize, squareSize);

			// Adding lettering and numbering on the sides
			if(x == 0){
				ctx.textAlign = "left";
				ctx.textBaseline = "top";
				color = (((x + y) % 2 != 0) ? light : dark);
				ctx.fillStyle = "rgb("+color.r+", "+color.g+", "+color.b+")";
				ctx.font = "24px Arial";
				if(!blackView){
					ctx.fillText(8-y, x * squareSize, y * squareSize);
				} else {
					ctx.fillText(y+1, x * squareSize, y * squareSize);
				}
			}
			if(y == 7){
				ctx.textAlign = "left";
				ctx.textBaseline = "bottom";
				color = (((x + y) % 2 != 0) ? light : dark);
				ctx.fillStyle = "rgb("+color.r+", "+color.g+", "+color.b+")";
				ctx.font = "24px Arial";
				if(!blackView){
					ctx.fillText(String.fromCharCode(x+97), x * squareSize, (y+1) * squareSize);
				} else {
					ctx.fillText(String.fromCharCode(7-x+97), x * squareSize, (y+1) * squareSize);
				}
			}
		}
	}
	drawPieces();
}

function trackmouse(event){
	let canvas = document.getElementById("canv");
	let offset = cumulativeOffset(canvas);
	let mouseX = event.clientX - offset.left;
	let mouseY = event.clientY - offset.top;

	let col = Math.floor(mouseX / squareSize);
	let row = Math.floor(mouseY / squareSize);

	mouseCol = col;
	mouseRow = row;

	selectSquare();

	// // TESTING
	// specialSquares = [];
	// let l1 = [];
	// let l2 = [];
	// let l3 = [];
	// let l4 = [];
	// l1 = getKSquare(rc2Index(mouseRow, mouseCol));
	// for(let x = 0 ; x < l1.length ; x++){
	// 	l2.push(...getKSquare(l1[x]));
	// }
	// for(let x = 0 ; x < l2.length ; x++){
	// 	l3.push(...getKSquare(l2[x]));
	// }
	// for(let x = 0 ; x < l3.length ; x++){
	// 	l4.push(...getKSquare(l3[x]));
	// }
	// specialSquares = l4;

}

var getKSquare = function(i){
	let [r,c] = index2RC(i);
	let avail = [];
	let a = [-1, 1];
	for(let x = 0 ; x < a.length ; x++){
		for(let y = 0 ; y < a.length ; y++){
			a1 = r + 2 * a[x];
			a2 = c + 1 * a[y];
			a3 = r + 1 * a[x];
			a4 = c + 2 * a[y];
			if(a1 < 8 && a1 > -1 && a2 < 8 && a2 > -1){
				avail.push(rc2Index(a1, a2));
			}
			if(a3 < 8 && a3 > -1 && a4 < 8 && a4 > -1){
				avail.push(rc2Index(a3, a4));
			}
		}
	}
	return avail;
	// specialSquares.push(rc2Index(mouseRow + 2 * r[x], mouseCol + 1 * c[y]));
	// specialSquares.push(rc2Index(mouseRow + 1 * r[x], mouseCol + 2 * c[y]));
}

var selectSquare = function(){
	// console.log(rc2Index(mouseRow, mouseCol));
	if(selSquare == -1){
		selSquare = rc2Index(mouseRow, mouseCol);
	} else {
		selSquare = blackView ? blackIndex(selSquare) : selSquare;
		let piece = pieces[selSquare];
		pieces[selSquare] = "-";
		let targetSquare = rc2Index(mouseRow,mouseCol);
		targetSquare = blackView ? blackIndex(targetSquare) : targetSquare;
		pieces[targetSquare] = piece;
		selSquare = -1;
	}
}

var cumulativeOffset = function(element) {
	var top = 0, left = 0;
	do {
		top += element.offsetTop || 0;
		left += element.offsetLeft || 0;
		element = element.offsetParent;
	} while(element);

	return {
		top: top,
		left: left
	};
};

var blackIndex = function(i){
	return pieces.length - 1 - i;
}

var square = function(row, col){
	return String.fromCharCode(col+97) + (8-row);
}

var rc2Index = function(row, col){
	return 8 * row + col;
}

var index2RC = function(i){
	let c = i % 8;
	let r = (i-c) / 8;
	return [r,c];
}

var setFEN = function(fen){
	let piece;
	let rank = 8;
	let file = 1;
	for(let i = 0 ; i < fen.length && rank > 0 ; i++){
		let count = 1;
		switch(fen[i]){
			case "p":
			case "r":
			case "n":
			case "b":
			case "q":
			case "k":
			case "P":
			case "R":
			case "N":
			case "B":
			case "Q":
			case "K":
				piece = fen[i];
				break;
			case "1":
			case "2":
			case "3":
			case "4":
			case "5":
			case "6":
			case "7":
			case "8":
				piece = "-"
				count = parseInt(fen[i]);
				break;
			case "/":
			case " ":
				rank--;
				file = 1;
				continue;
			default:
				console.log("Error with FEN!");
		}
		for(let x = 0 ; x < count ; x++){
			let boardIndex = 8 * (8-rank) + file - 1;
			pieces[boardIndex] = piece;
			file++;
		}
	}
	let stateInfo = fen.split(' ');
	side = stateInfo[1] == "b" ? "black" : "white";
	castlePermission = stateInfo[2];
	enPas = stateInfo[3];
	updateSide();
}

// This can be written better
var genFEN = function(){
	let fen = '';
	let empty = 0;
	for(let i = 0 ; i < pieces.length ; i++){
		if(i % 8 == 0 && i > 0){
			if(empty){ fen += empty; empty = 0; }
			fen += "/";
		}
		if(pieces[i] == "-"){
			empty++;
		} else {
			if(empty){ fen += empty; empty = 0; }
			fen += pieces[i];
		}
	}
	if(empty){ fen += empty; empty = 0; }
	fen += ' ';
	fen += side == "black" ? "b" : "w";
	fen += ' ';
	fen += castlePermission;
	fen += ' ';
	fen += enPas;

	return fen;
}

var drawPieces = function(){
	let canv = document.getElementById("canv");
	let ctx = canv.getContext("2d");
	let path;
	for(let i = 0 ; i < pieces.length ; i++){
		switch(pieces[i]){
			case "p": path = "./pieces/pawn_black_60.png"; break;
			case "r": path = "./pieces/rook_black_60.png"; break;
			case "n": path = "./pieces/night_black_60.png"; break;
			case "b": path = "./pieces/bishop_black_60.png"; break;
			case "q": path = "./pieces/queen_black_60.png"; break;
			case "k": path = "./pieces/king_black_60.png"; break;
			case "P": path = "./pieces/pawn_white_60.png"; break;
			case "R": path = "./pieces/rook_white_60.png"; break;
			case "N": path = "./pieces/night_white_60.png"; break;
			case "B": path = "./pieces/bishop_white_60.png"; break;
			case "Q": path = "./pieces/queen_white_60.png"; break;
			case "K": path = "./pieces/king_white_60.png"; break;
			default: path = null;
		}
		if(path){
			let imgObj = new Image();
			imgObj.src = path;
			let x = blackView ? 7 - i % 8 : i % 8;
			let y = blackView ? (56-i+(i%8)) / 8 : (i-x) / 8;
			ctx.drawImage(
				imgObj,
				(x + 0.5) * squareSize - imgObj.width / 2,
				(y + 0.5) * squareSize - imgObj.height / 2
			);
		}
	}
}

function formatPOSTData(object) {
	var encodedString = '';
	for (var key in object) {
		if (object.hasOwnProperty(key)) {
			if (encodedString.length > 0) {
				encodedString += '&';
			}
			encodedString += encodeURI(key + '=' + object[key]);
		}
	}
	return encodedString;
}

var randMove = function(){
	let fen = genFEN();
	let xhr = new XMLHttpRequest();
	let url = "http://192.168.1.90:5812/fen?fen=" + fen;
	console.log("req: " + url);

	xhr.open('GET', url);
	xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xhr.onload = function(){
		if (xhr.status !== 200){
			alert('Something went wrong with submission.  Please contact Brendan.')
		} else {
			// success
			let randFEN = xhr.responseText.split('\n')[0];
			console.log("res: " + randFEN);
			if(randFEN == fen){
				alert("MATE");
			} else {
				let input = document.querySelectorAll("form input.input")[0];
				input.value = randFEN;
				setFEN(randFEN);
			}
		}
	};
	xhr.send();
}

var saveFEN = function(){
	let fen = genFEN();
	let url = './handle_input.php';

	let xhr = new XMLHttpRequest();
	xhr.open('POST', url);
	xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xhr.onload = function(){
		if (xhr.status !== 200){
			alert('Something went wrong with submission.  Please contact Brendan.')
		} else {
			alert("Successfully saved board!")
		}
		console.log(xhr.status);
	};
	let payload = {
		fen: fen
	};
	xhr.send(formatPOSTData(payload));
}

var loadFEN = function(){
	let url = './handle_input.php';

	let xhr = new XMLHttpRequest();
	xhr.open('GET', url);
	xhr.onload = function(){
		if (xhr.status !== 200){
			alert('Something went wrong with submission.  Please contact Brendan.')
		}
		console.log(xhr.status);
	};
	xhr.onload = function() {
		if (xhr.status != 200) { // analyze HTTP status of the response
			alert(`Error ${xhr.status}: ${xhr.statusText}`); // e.g. 404: Not Found
		} else { // show the result
			// alert(`Data: ${xhr.response}`); // responseText is the server
			let res = JSON.parse(xhr.response);
			fen = res.fen;
			setFEN(fen);
		}
	};
	xhr.send();
}

var resetFEN = function(){
	fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	setFEN(fen);
}

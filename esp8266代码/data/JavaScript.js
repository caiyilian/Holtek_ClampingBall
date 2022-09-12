


// 初始化AJAX，用于发送请求
function ajax(opt) {
    opt = opt || {};
    opt.method = opt.method.toUpperCase() || 'POST';
    opt.url = opt.url || '';
    opt.async = opt.async || true;
    opt.data = opt.data || null;
    opt.success = opt.success || function () {};
    var xmlHttp = null;
    if (XMLHttpRequest) {
        xmlHttp = new XMLHttpRequest();
    }
    else {
        xmlHttp = new ActiveXObject('Microsoft.XMLHTTP');
    }var params = [];
    for (var key in opt.data){
        params.push(key + '=' + opt.data[key]);
    }
    var postData = params.join('&');
    if (opt.method.toUpperCase() === 'POST') {
        xmlHttp.open(opt.method, opt.url, opt.async);
        xmlHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded;charset=utf-8');
        xmlHttp.send(postData);
    }
    else if (opt.method.toUpperCase() === 'GET') {
        xmlHttp.open(opt.method, opt.url + '?' + postData, opt.async);
        xmlHttp.send(null);
    } 
    xmlHttp.onreadystatechange = function () {
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
            opt.success(xmlHttp.responseText);//如果不是json数据可以去掉json转换
        }
    };
}


// 小车的模式选择
var mode_buttom = document.getElementById('mode')
var mode_label = document.getElementsByClassName('switch-box-label')
mode_buttom.onchange = function(){
    isCheack = mode_buttom.checked
    mode_label[0].innerHTML = isCheack?'手动模式':'自动模式'
    ajax({
        method:'POST',
        url:"/ChangeMode",
        data:{mode:isCheack},
        success:function (){
            let right_contents = document.getElementsByClassName('right_content')
            if(isCheack){
                right_contents[0].style.display = 'block'
                right_contents[1].style.display = 'none'
            }else{
                right_contents[0].style.display = 'none'
                right_contents[1].style.display = 'block'
            }
        }
    })//发送请求
}



//获取电量
var battery_nodes = document.getElementsByClassName('value');
let battery_timer = setInterval(() => {
    ajax({
        method:'GET',
        url:"/ShowBattery",
        success:function(battery_value_str){
            for(let i=0;i<4;i++){
                if (i<Number(battery_value_str)) battery_nodes[i].style.backgroundColor="rgb(30, 242, 62)"
                else battery_nodes[i].style.backgroundColor="gray"
            }
        }
    })//发送请求
}, 5000000);

// 获取方向盘
var dirs = document.getElementsByClassName('iconfont')
for(let i=0;i<dirs.length;i++){
    dirs[i].onclick = function(){
        
        ajax({
            method:'POST',
            url:"/CarDirect",
            data:{direct:dirs[i].id}
        })//发送请求
    }
}
//获取小车状态和夹球数量
var carState = document.getElementsByClassName('car_state')
var ballNumber = document.getElementsByClassName('ball_number')
let carState_timer = setInterval(() => {
    ajax({
        method:'GET',
       url:"/CarSate",
       success:function(car_state){
           carState[0].innerHTML=JSON.parse(car_state).state;
           ballNumber[0].innerHTML=String(JSON.parse(car_state).number)
       }
})
}, 200);





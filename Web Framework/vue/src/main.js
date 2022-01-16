import Vue from 'vue'
import App from './App.vue'
import router from './router'
import './plugins/element.js'
import './plugins/v_charts.js'
import './plugins/utils.js'
import './plugins/table.js'
// 导入字体图标
import './assets/fonts/iconfont.css'

// 导入全局样式css
import './assets/css/global.css'
import './assets/vxe/vxetable.scss'

import axios from 'axios'
import VueAxios from 'vue-axios'

import VueApexCharts from 'vue-apexcharts'
import store from './store'
Vue.use(VueApexCharts)

Vue.component('apexchart', VueApexCharts)

// 配置请求根路径
// 本机地址
axios.defaults.baseURL = 'http://127.0.0.1:5100'
// 远程后台地址
// axios.defaults.baseURL = 'https://www.liulongbin.top:8888/api/private/v1/'
axios.defaults.headers.post['Content-Type'] = 'application/x-www-form-urlencoded; charset=UTF-8'
Vue.use(VueAxios, axios)

Vue.config.productionTip = false
new Vue({
  router,
  store,
  render: h => h(App)
}).$mount('#app')

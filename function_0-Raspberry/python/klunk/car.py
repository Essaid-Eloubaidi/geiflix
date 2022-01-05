from pickle import NONE
import klunk as k
import klunk.can as can
import klunk.motors as motors
import klunk.ultrasound as us
import klunk.lidar as lidar
import time

class Car:

    IDLE   = 0
    AUTO   = 1
    MANUAL = 2
    STOP   = 3
    UNSAFE = 4
    MODES  = [IDLE, AUTO, MANUAL, STOP, UNSAFE]

    def __init__(self, can_bus):
        #init engine
        self._speed = motors.SPEED_STOP
        self._steer = motors.STEER_STRAIGHT
        self.can_bus = can_bus
        self.send_motors_order()
        #init sensors
        self.US = us.Ultrasound()
        self.lidar = lidar.Lidar()
        self.lidar.start()
        #init status
        self.mode = Car.IDLE

    def ready(self):
        WAIT_TIME = 0.5
        self.steer = motors.STEER_LEFT_CLOSE
        time.sleep(WAIT_TIME)
        self.steer = motors.STEER_RIGHT_CLOSE
        time.sleep(WAIT_TIME)
        self.steer = motors.STEER_STRAIGHT

    def send_motors_order(self):
        self.can_bus.send(can.motors_message(self.speed, self.steer))

#engine commands
    @property
    def speed(self):
        return self._speed

    @speed.setter
    def speed(self, speed):
        if speed in motors.SPEEDS:
            self._speed = speed
            self.send_motors_order()
        else:
            raise ValueError("invalid speed value")

    def brake(self):
        self.speed = motors.SPEED_STOP

    def faster(self):
        self.speed = motors.faster(self.speed)

    def slower(self):
        self.speed = motors.slower(self.speed)

    @property
    def steer(self):
        return self._steer

    @steer.setter
    def steer(self, steer):
        if steer in motors.STEERS:
            self._steer = steer
            self.send_motors_order()
        else:
            raise ValueError("invalid steer value")

    def lefter(self):
        self.steer = motors.lefter(self.steer)

    def righter(self):
        self.steer = motors.righter(self.steer)

#car status
    @property
    def mode(self):
        return self._mode

    @mode.setter
    def mode(self, value):
        if(value in Car.MODES):
            self._mode = value
            with open('/var/www/Klunk/mode.txt', 'w') as file:
                file.write(["IDLE", "AUTO", "MANUAL", "STOP", "UNSAFE"][self.mode])
        else:
            raise ValueError("Invalid mode")

    def is_stopped(self):
        return self.speed == motors.SPEED_STOP

    def is_going_forward(self):
        return self.speed > motors.SPEED_STOP

    def is_going_backward(self):
        return self.speed < motors.SPEED_STOP

    def is_going_straight(self):
        return self.steer == motors.STEER_STRAIGHT

    def is_going_left(self):
        return self.steer < motors.STEER_STRAIGHT

    def is_going_right(self):
        return self.steer > motors.STEER_STRAIGHT

    #sensor commands
    def update_us(self, message):
        self.US.update(message)

    ##### avoidance old treatment


    def is_safe(self):
        if self.is_going_forward():
            if self.is_going_straight() and self.US.front_obstacle():
                return False
            elif self.is_going_left() and (self.US.front_left_obstacle() or self.US.front_center_obstacle()):
                return False
            elif self.is_going_right() and (self.US.front_right_obstacle() or self.US.front_center_obstacle()):
                return False
        elif self.is_going_backward() and self.US.rear_obstacle():
            return False

        return True

function dxdt = ode_fun(t,x)

    global fThrust fMass fGrav massBody launchFlag

    mass   = massBody + fMass(t);
    thrust = fThrust(t);
    grav   = fGrav(x(1));
    
    force = thrust - mass*grav;
    
    if (force < 0 && ~launchFlag)
        force = 0;
    elseif ~launchFlag
        launchFlag  = true;
    end
    
    dxdt_1 = x(2);
    dxdt_2 = force / mass;

    dxdt = [dxdt_1; dxdt_2];

end